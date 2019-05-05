#include "aesencryptor.h"

#include <stdio.h>
#include <cstring>
#include <iostream>

#include "common/printf.h"

unsigned char AESEncryptor::initVec[AES_BLOCK_SIZE] = {0};
AES_KEY AESEncryptor::encryptKey;
AES_KEY AESEncryptor::decryptKey;
int AESEncryptor::hasSetParams = 0;

const char paddingChar = 0;

AESEncryptor::AESEncryptor()
{

}

AESEncryptor::~AESEncryptor()
{

}

void AESEncryptor::SetDefaultInitVec()
{
    SetInitVec("B1005B1005");
}

void AESEncryptor::SetInitVec(const std::string &vecStr)
{
    memset(initVec, 0, AES_BLOCK_SIZE);//clear

    //截取AES_BLOCK_SIZE大小的initVec
    memcpy(initVec, vecStr.c_str(), std::min(vecStr.length(), std::string::size_type(AES_BLOCK_SIZE)));

    hasSetParams |= 0x01;
}

int AESEncryptor::SetKey(const std::string &key)
{
    //截取AES_BLOCK_SIZE大小的key
    unsigned char userKey[AES_BLOCK_SIZE] = {0};
    memcpy(userKey, key.c_str(), std::min(key.length(), (std::string::size_type)AES_BLOCK_SIZE));

    //对称加密 key=encryptKey和decryptKey
    int ret = AES_set_encrypt_key(userKey, AES_BLOCK_SIZE*8, &encryptKey);//字节转为位数=*8
    if(ret<0){//set key error
        PRINTF(LOG_ERR, "%s: set encrypt key error(%s)!\n", __func__, ret==-1? "userKey is NULL": "key length is not 128/198/256");
        return -1;
    }
    ret = AES_set_decrypt_key(userKey, AES_BLOCK_SIZE*8, &decryptKey);
    if(ret<0){//set key error
        PRINTF(LOG_ERR, "%s: set decrypt key error(%s)!\n", __func__, ret==-1? "userKey is NULL": "key length is not 128/198/256");
        return -1;
    }

    hasSetParams |= 0x10;
    return 0;
}

int AESEncryptor::Encrypt(const std::string &msg, std::string &encryptedMsg)
{
    return ProcessMsg(AES_ENCRYPT, msg, encryptedMsg);
}

int AESEncryptor::Decrypt(const std::string &msg, std::string &decryptedMsg)
{
    if(ProcessMsg(AES_DECRYPT, msg, decryptedMsg) == -1){
        return -1;
    }

    //正常解密 则剔除末尾的NULL
    TrimFromEnd(decryptedMsg);
    return 0;
}

int AESEncryptor::ProcessMsg(const int method, const std::string &msg, std::string &processedMsg)
{
    //检查密钥和向量设定
    if( !CheckParamsSetting() ){
        PRINTF(LOG_ERR, "%s: params have not been set!\n", __func__);
        return -1;
    }
    //设定密钥
    AES_KEY aesKey = (method==AES_ENCRYPT? encryptKey: decryptKey);
    //设定向量
    unsigned char ivec[AES_BLOCK_SIZE];//加密的初始化向量IV
    memcpy(ivec, initVec, AES_BLOCK_SIZE);//每次使用时拷贝(AES_cbc_encrypt会改变ivec的值)

    const std::string::size_type blockCount = msg.length()/AES_BLOCK_SIZE;
    processedMsg.clear();
    processedMsg.reserve( (blockCount+1)*AES_BLOCK_SIZE );//+1预留分组补足的部分(加快执行速度)

    //AES_BLOCK_SIZE大小为块 进行分块加密
    unsigned char outBuf[AES_BLOCK_SIZE] = {paddingChar};
    for(std::string::size_type i=0; i<blockCount; ++i){
        //取AES_BLOCK_SIZE大小 进行加密
        const unsigned char* inBuf = (const unsigned char*)(msg.c_str() + i*AES_BLOCK_SIZE);
        AES_cbc_encrypt(inBuf, outBuf, AES_BLOCK_SIZE, &aesKey, ivec, method);

        processedMsg.append((const char*)outBuf, AES_BLOCK_SIZE);
    }

    const int leftLen = msg.length()%AES_BLOCK_SIZE;
    if(leftLen != 0){//最后不足AES_BLOCK_SIZE大小的块
        //补足AES_BLOCK_SIZE大小(填充paddingChar) 再进行加密
        unsigned char inBuf[AES_BLOCK_SIZE] = {paddingChar};
        memcpy(inBuf, msg.c_str()+msg.length()-leftLen, leftLen);
        AES_cbc_encrypt(inBuf, outBuf, AES_BLOCK_SIZE, &aesKey, ivec, method);

        processedMsg.append((const char*)outBuf, AES_BLOCK_SIZE);
    }

    return 0;
}

bool AESEncryptor::CheckParamsSetting()
{
    int ret = hasSetParams&0x11;
    if(ret==0x11){
        return true;
    }

    if((ret&0x10) == 0){
        PRINTF(LOG_ERR, "%s: key has not been set.\n", __func__);
    }
    if((ret&0x01) == 0){
        PRINTF(LOG_ERR, "%s: ivec has not been set.\n", __func__);
    }
    return false;
}

void AESEncryptor::TrimFromEnd(std::string &msg)
{
    std::string::iterator itr = msg.end()-1;
    for( ; itr!=msg.begin(); --itr){
        if(*itr != paddingChar){//最后一个非paddingChar字符
            break;
        }
    }
    msg.erase(itr+1, msg.end());
}
