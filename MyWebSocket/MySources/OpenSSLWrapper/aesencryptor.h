#ifndef AESENCRYPTOR_H
#define AESENCRYPTOR_H

#include <string>
#include <openssl/aes.h>

class AESEncryptor
{
public:
    AESEncryptor();
    ~AESEncryptor();

    //Set必须在加解密之前设定
    static void SetDefaultInitVec();
    static void SetInitVec(const std::string& vecStr);
    static int SetKey(const std::string& key);

    //加密
    static int Encrypt(const std::string& msg, std::string& encryptedMsg);
    //解密
    static int Decrypt(const std::string& msg, std::string& decryptedMsg);

private:
    static int ProcessMsg(const int method, const std::string &msg, std::string &processedMsg);
    static bool CheckParamsSetting();
    static void TrimFromEnd(std::string& msg);

private:
    static unsigned char initVec[AES_BLOCK_SIZE];//加密的初始向量
    static AES_KEY encryptKey;
    static AES_KEY decryptKey;

    static int hasSetParams;//不同位上置一 表示向量/密钥已经设置
};

#endif // AESENCRYPTOR_H
