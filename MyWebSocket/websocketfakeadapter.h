#ifndef WEBSOCKETFAKEADAPTER_H
#define WEBSOCKETFAKEADAPTER_H

#include <string>

/* 虚假的WebSocket适配器
 * 该类提供WebSocketAdapter的所有public接口 但是不执行任何有效操作
 *
 * 目的:
 *   在最小化代码改动的情况下, 使得本地UI客户端可以不使用WebSocket进行通讯
 *   在CommunicationMediator中只需要修改Adapter指针的静态类型以及new的构造语句即可
 *   其他代码均不需要修改
 *
 * 使用:
 *   MyWebSocket.pri中ENABLE_WEBSOCKET_COMMUNICATION宏是否存在 来控制是否使能WebSocket的通讯
 * */

class WebSocketFakeAdapter
{
public:
    explicit WebSocketFakeAdapter(const std::string& groupId, const std::string& groupPassword, const std::string& rsaPublicKeyFile);
    virtual ~WebSocketFakeAdapter();

    void Start(const std::string& serverIp, const uint16_t port);
    void Stop();

    template<class T>
    int SendMsg(const T& msg){
        Q_UNUSED(msg);
        return 0;
    }

    int CheckMsg();
    template<class T>
     void ReceiveMsg(T& msg){
         Q_UNUSED(msg);
     }
};

#endif // WEBSOCKETFAKEADAPTER_H
