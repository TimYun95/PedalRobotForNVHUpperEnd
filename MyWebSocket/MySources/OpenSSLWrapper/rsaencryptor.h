#ifndef RSAENCRYPTOR_H
#define RSAENCRYPTOR_H

/* 非对称加密 加密数据的长度存在限制(与密钥长度有关) */

#include <string>
#include <openssl/rsa.h>

class RSAEncryptor
{
public:
    explicit RSAEncryptor();
    ~RSAEncryptor();

    /* 读取密钥 */
    static void ReadPublicKey(const std::string& filePath);
    static void ReadPrivateKey(const std::string& filePath);
    static void FreeKeys();

    //加密
    static int EncryptViaPublicKey(const std::string &msg, std::string &encryptedMsg);
    static int EncryptViaPrivateKey(const std::string &msg, std::string &encryptedMsg);

    //解密
    static int DecryptViaPublicKey(const std::string &msg, std::string &decryptedMsg);
    static int DecryptViaPrivateKey(const std::string &msg, std::string &decryptedMsg);

private:
    enum EncrypyKey{
        UsePublicKey,
        UsePrivateKey
    };

    static int RSAEncrypt(const std::string& msg, std::string& encryptedMsg, const EncrypyKey keyType);

    static int RSADecrypt(const std::string& msg, std::string& decryptedMsg, const EncrypyKey keyType);

    static bool CheckKeyValid(const EncrypyKey keyType);

private:
    //RSA密钥
    static RSA* publicKey;
    static RSA* privateKey;
    //RSA密钥长度
    static int keyLen;
};

#endif // RSAENCRYPTOR_H
