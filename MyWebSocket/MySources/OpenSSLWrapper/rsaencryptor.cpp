#include "rsaencryptor.h"

#include <stdlib.h>

#include <openssl/pem.h>
#include <openssl/err.h>

#ifdef WIN32
//处理no OPENSSL_Applink的错误(applink.c必须添加到编译器内)
#ifndef APPMACROS_ONLY//APPMACROS_ONLY在applink.c中使用到
 #define APPMACROS_ONLY
 extern "C"{
  #include <openssl/applink.c>
 }
#endif
#endif

#include "common/printf.h"

RSA* RSAEncryptor::publicKey  = NULL;
RSA* RSAEncryptor::privateKey = NULL;
int RSAEncryptor::keyLen  = 0;

RSAEncryptor::RSAEncryptor()
{

}

RSAEncryptor::~RSAEncryptor()
{

}

void RSAEncryptor::ReadPublicKey(const std::string &filePath)
{
    PRINTF(LOG_DEBUG, "%s: start...\n", __func__);
    FILE* file = fopen(filePath.c_str(), "r");
    if(file == NULL){
        PRINTF(LOG_ERR, "%s: cannot open file=%s\n", __func__, filePath.c_str());
        return;
    }

    publicKey = PEM_read_RSA_PUBKEY(file, NULL, NULL, NULL);
    if(publicKey == NULL){
        ERR_print_errors_fp(stdout);
        PRINTF(LOG_ERR, "%s: PEM_read_RSA_PUBKEY error.\n", __func__);
    }else{
        keyLen = RSA_size(publicKey);
        PRINTF(LOG_DEBUG, "%s: keyLen=%d\n", __func__, keyLen);
    }

    fclose(file);
}

void RSAEncryptor::ReadPrivateKey(const std::string &filePath)
{
    PRINTF(LOG_DEBUG, "%s: start...\n", __func__);
    FILE* file = fopen(filePath.c_str(), "r");
    if(file == NULL){
        PRINTF(LOG_ERR, "%s: cannot open file=%s\n", __func__, filePath.c_str());
        return;
    }

    privateKey = PEM_read_RSAPrivateKey(file, NULL, NULL, NULL);
    if(privateKey == NULL){
        ERR_print_errors_fp(stdout);
        PRINTF(LOG_ERR, "%s: PEM_read_RSAPrivateKey error.\n", __func__);
    }else{
        keyLen = RSA_size(privateKey);
        PRINTF(LOG_DEBUG, "%s: keyLen=%d\n", __func__, keyLen);
    }

    fclose(file);
}

void RSAEncryptor::FreeKeys()
{
    if(publicKey){
        RSA_free(publicKey);
    }
    if(privateKey){
        RSA_free(privateKey);
    }

    publicKey = NULL;
    privateKey = NULL;
    keyLen = 0;
}

int RSAEncryptor::EncryptViaPublicKey(const std::string &msg, std::string &encryptedMsg)
{
    return RSAEncrypt(msg, encryptedMsg, UsePublicKey);
}

int RSAEncryptor::EncryptViaPrivateKey(const std::string &msg, std::string &encryptedMsg)
{
    return RSAEncrypt(msg, encryptedMsg, UsePrivateKey);
}

int RSAEncryptor::DecryptViaPublicKey(const std::string &msg, std::string &decryptedMsg)
{
    return RSADecrypt(msg, decryptedMsg, UsePublicKey);
}

int RSAEncryptor::DecryptViaPrivateKey(const std::string &msg, std::string &decryptedMsg)
{
    return RSADecrypt(msg, decryptedMsg, UsePrivateKey);
}

int RSAEncryptor::RSAEncrypt(const std::string &msg, std::string &encryptedMsg, const EncrypyKey keyType)
{
    if( !CheckKeyValid(keyType) ){
        return -1;
    }

    std::string::size_type maxMsgLen = keyLen-11;//最大明文消息长度=密钥长度(Bytes)-11
    if(msg.length() > maxMsgLen){
        PRINTF(LOG_ERR, "%s: error. msg is too long (%d/%u) for rsa algorithm!\n", __func__, msg.length(), maxMsgLen);
        return -1;
    }

    const unsigned char* inBuf = (const unsigned char*)msg.c_str();
    unsigned char outBuf[keyLen] = {0};
    int ret;//返回加密字节数
    if(keyType==UsePublicKey){
        ret = RSA_public_encrypt(msg.length(), inBuf, outBuf, publicKey, RSA_PKCS1_PADDING);
    }else{//UsePrivateKey
        ret = RSA_private_encrypt(msg.length(), inBuf, outBuf, privateKey, RSA_PKCS1_PADDING);
    }

    if(ret < 0){
        PRINTF(LOG_ERR, "%s: RSA_%s_encrypt error!\n", __func__, keyType==UsePublicKey? "public": "private");
        return -1;
    }else{
        PRINTF(LOG_DEBUG, "%s: RSA_%s_encrypt %d byte(s)\n", __func__, keyType==UsePublicKey? "public": "private", ret);
    }

    encryptedMsg.assign((const char*)outBuf, ret);//assign 防止加密出现的'\0'截断字符串
    return 0;
}

int RSAEncryptor::RSADecrypt(const std::string &msg, std::string &decryptedMsg, const EncrypyKey keyType)
{
    if( !CheckKeyValid(keyType) ){
        return -1;
    }

    if(msg.length() != std::string::size_type(keyLen)){//RSA_PKCS1_PADDING填充 加密后的消息长度=密钥长度
        PRINTF(LOG_ERR, "%s: error. msg len=%d/%d\n", __func__, msg.length(), keyLen);
        return -1;
    }

    const unsigned char* inBuf = (const unsigned char*)msg.c_str();
    unsigned char outBuf[keyLen] = {0};
    int ret;//返回解密字节数
    if(keyType==UsePublicKey){
        ret = RSA_public_decrypt(msg.length(), inBuf, outBuf, publicKey, RSA_PKCS1_PADDING);
    }else{//UsePrivateKey
        ret = RSA_private_decrypt(msg.length(), inBuf, outBuf, privateKey, RSA_PKCS1_PADDING);
    }

    if(ret < 0){
        PRINTF(LOG_ERR, "%s: RSA_%s_decrypt error!\n", __func__, keyType==UsePublicKey? "public": "private");
        return -1;
    }else{
        PRINTF(LOG_DEBUG, "%s: RSA_%s_decrypt %d byte(s)\n", __func__, keyType==UsePublicKey? "public": "private", ret);
    }

    decryptedMsg.assign((const char*)outBuf, ret);//assign 防止解密出现的'\0'截断字符串(对于字符串 \0是终止符; 对于JSON字符串 \0也可能是数值0)
    return 0;
}

bool RSAEncryptor::CheckKeyValid(const RSAEncryptor::EncrypyKey keyType)
{
    if(keyType==UsePublicKey && publicKey == NULL){
        PRINTF(LOG_ERR, "%s: error. publicKey is NULL!\n", __func__);
        return false;
    }
    if(keyType==UsePrivateKey && privateKey == NULL){
        PRINTF(LOG_ERR, "%s: error. privateKey is NULL!\n", __func__);
        return false;
    }
    return true;
}
