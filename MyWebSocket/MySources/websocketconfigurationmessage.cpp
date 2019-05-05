#include "websocketconfigurationmessage.h"

#include "MyWebSocket/MySources/JsonWrapper/jsonwrapper.h"
#include "MyWebSocket/MySources/OpenSSLWrapper/rsaencryptor.h"

#define GROUP_ID       "GroupId"
#define GROUP_PASSWORD "GroupPassword"

WebSocketConfigurationMessage::WebSocketConfigurationMessage()
    : groupId(), groupPassword(), valid(false)
{

}

WebSocketConfigurationMessage::WebSocketConfigurationMessage(const std::string &id, const std::string &password)
    : groupId(id), groupPassword(password), valid(false)
{

}

WebSocketConfigurationMessage::~WebSocketConfigurationMessage()
{

}

bool WebSocketConfigurationMessage::ValidMessage()
{
    return valid;
}

std::string WebSocketConfigurationMessage::Serialize()
{
    //序列化
    JSONWrapper json;
    json.addString(GROUP_ID, groupId);
    json.addString(GROUP_PASSWORD, groupPassword);
    const std::string jsonStr = json.toString();

    //客户端使用公钥加密WS配置信息
    std::string encryptedMsg;
    if(RSAEncryptor::EncryptViaPublicKey(jsonStr, encryptedMsg) == -1){
        return "";
    }

    return encryptedMsg;
}

void WebSocketConfigurationMessage::Deserialize(const std::string& msg)
{
    //服务器端使用使用私钥解密WS配置信息
    std::string decryptedMsg;
    if(RSAEncryptor::DecryptViaPrivateKey(msg, decryptedMsg) == -1){
        valid = false;
        return;
    }

    //反序列化
    JSONWrapper json;
    int ret = json.Parse(decryptedMsg);
    if(ret == -1){
        valid = false;
        return;
    }

    json.loadString(GROUP_ID, groupId);
    json.loadString(GROUP_PASSWORD, groupPassword);
    valid = true;
}

const std::string& WebSocketConfigurationMessage::GetGroupId()
{
    return groupId;
}

const std::string& WebSocketConfigurationMessage::GetGroupPassword()
{
    return groupPassword;
}
