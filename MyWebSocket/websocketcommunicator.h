#ifndef WEBSOCKETCOMMUNICATOR_H
#define WEBSOCKETCOMMUNICATOR_H

#include <string>
#include <mutex>

#include "MyWebSocket/mytypedefine.h"
#include "MyWebSocket/MySources/websocketclient.h"

#include "MyWebSocket/MySources/websocketmsgqueue.h"
#include "MyWebSocket/MySources/basicsemaphore.h"

class WebSocketMessage;

/* WebSocket通讯器
 * 负责消息收发时:
 * 1) 加解密消息(AESEncryptor)
 * 2) 格式化消息格式(WebSocketMessage)
 * */

class WebSocketCommunicator
{
public:
    explicit WebSocketCommunicator(const std::string& groupId, const std::string& groupPassword, const std::string& rsaPublicKeyFile);
    virtual ~WebSocketCommunicator();

    void Start(const std::string& serverIp, const uint16_t port);
    void Stop();

protected:
    //以下函数将被子类WebSocketAdapter访问 适配收发函数接口
    void SendWebSocketMsg(const std::string& msg);
    bool CheckRecvMsg(shared_string_ptr &msg);

private:
    //线程函数
    void Run();

    //以下函数将被友元类WebSocketClient访问
    friend class WebSocketClient;
    void OnConnectionEstablished();
    void OnConnectionClosed();
    void OnReceiveBinaryMsg(websocketpp::connection_hdl handler, const std::string& msg);

    void ReceiveConfMsg(const std::string& encryptedKey);
    void ReceiveForwardMsg(const WebSocketMessage& wsMsg);
    void SendCloseMsg();
    void SetSrcDstGroupId(WebSocketMessage& wsMsg);
    bool CheckConnection();

private:
    //WebSocket客户端
    WebSocketClient wsClient;

    //接收队列相关
    WebSocketMsgQueue recvQueue;
    std::mutex recvMutex;
    //发送队列相关
    WebSocketMsgQueue sendQueue;
    std::mutex sendMutex;
    BasicSemaphore sendSem;

    //当前状态
    int status;
    enum ClientStatus{
        Error,
        Stopped,
        Running,
        Established,
        WaitForAesKey
    };

    //常量成员
    const std::string wsGroupId;
    const std::string wsGroupPassword;
};

#endif // WEBSOCKETCOMMUNICATOR_H
