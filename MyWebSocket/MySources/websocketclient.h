#ifndef WEBSOCKETCLIENT_H
#define WEBSOCKETCLIENT_H

#include <string>

#include <websocketpp/client.hpp>
#include <websocketpp/config/asio_no_tls_client.hpp>

/* WebSocket客户端 封装websocketpp库 进行最底层的数据收发
 *   和服务器建立连接后, 主要通过SendBinaryMessage()和OnReceiveMessage()进行消息的收发
 *
 * WebSocket消息走frame::opcode::value::binary通道
 * 不走frame::opcode::value::text通道 防止密文消息中的\0截断string */

class WebSocketCommunicator;

class WebSocketClient
{
public:
    explicit WebSocketClient(WebSocketCommunicator *wsCmt);
    virtual ~WebSocketClient();

    int ConnectServer(const std::string& hostAddress, const uint16_t port);
    void Stop();

    void SendTextMessage(const std::string& message);
    void SendBinaryMessage(const void *message, size_t len);
    void SendPingMessage(const std::string& message);

protected:
    void Run();
    void SetLogControl(bool enableLogger);
    void StartClientThread();
    void StopClientThread();

    std::string ConstructUri(const std::string& hostAddress, const uint16_t port);
    bool CheckStatus();

    /* 为使用到的一些类型定义别名 */
    typedef websocketpp::client<websocketpp::config::asio_client> WSPPClient;//WebSocket++Client
    typedef websocketpp::config::asio_client::message_type::ptr   message_ptr;

    /* WebSocket++的各种消息处理回调函数 */
    void OnOpenConnection(websocketpp::connection_hdl handler);
    void OnCloseConnection(websocketpp::connection_hdl handler);
    void OnFailConnection(websocketpp::connection_hdl handler);
    void OnReceiveMessage(websocketpp::connection_hdl handler, message_ptr msg);
    bool OnPingMessage(websocketpp::connection_hdl handler, std::string msg);
    bool OnPongMessage(websocketpp::connection_hdl handler, std::string msg);
    bool OnPongMessageTimeOut(websocketpp::connection_hdl handler, std::string msg);
    /* 1) The connection handle is a token that uniquely identifies the connection that received the message.
     * It can be used to identify where to send reply messages or stored and used to push messages later.
     * 2) there are internal loops in WebSocket++, so we just set the message handlers for different messages
     * */

    void ProcessTextMessage(websocketpp::connection_hdl handler, message_ptr msg);
    void ProcessBinaryMessage(websocketpp::connection_hdl handler, message_ptr msg);
    void ProcessUnknownMessage(websocketpp::connection_hdl handler, message_ptr msg);

    std::string GetHandlerInformation(websocketpp::connection_hdl handler, int informType);
    enum tagInformType{
        OpenInform  = 0x0001,
        CloseInform = 0x0002,
        FailInform  = 0x0004,
        PingInform  = 0x0008,
        PongInform  = 0x0010,
        PongTimeOut = 0x0020,
        UnknownMsg  = 0x0040
    };
    std::string GetConnectionUri(WSPPClient::connection_ptr connection);

protected:
    WSPPClient wsClient;
    websocketpp::lib::shared_ptr<websocketpp::lib::thread> wsClientThread;

    int status;
    enum tagStatus{
        Stopped,
        Running
    };

    WSPPClient::connection_ptr clientConnection;
    websocketpp::connection_hdl clientHandler;

    WebSocketCommunicator *cmt;
};

#endif // WEBSOCKETCLIENT_H
