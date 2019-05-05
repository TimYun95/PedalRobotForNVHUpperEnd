#ifndef WEBSOCKETCONFIGURATIONMESSAGE_H
#define WEBSOCKETCONFIGURATIONMESSAGE_H

#include <string>

/* WebSocket配置消息的封装类 在WS连接刚建立时使用
 * 1) 客户端: 连接建立后, 发送 群组ID及群组密码的RSA密文(公钥加密) 至服务器
 * 2) 服务器: 接收该消息, 验证群组信息; 验证通过后将发送 群组间通讯的AES密钥 至客户端
 * 该类用于保存 群组ID及群组密码的RSA密文
 * */

class WebSocketConfigurationMessage
{
public:
    WebSocketConfigurationMessage();
    WebSocketConfigurationMessage(const std::string& id, const std::string& password);
    ~WebSocketConfigurationMessage();

    bool ValidMessage();

    std::string Serialize();
    void Deserialize(const std::string &msg);

    const std::string& GetGroupId();
    const std::string& GetGroupPassword();

private:
    std::string groupId;
    std::string groupPassword;
    bool valid;
};

#endif // WEBSOCKETCONFIGURATIONMESSAGE_H
