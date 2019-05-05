#ifndef WEBSOCKETMSGTYPES_H
#define WEBSOCKETMSGTYPES_H

/* WebSocketMessage类中的msgType的枚举 */

namespace WSMsgTypes {
enum tagWebSocketMsgTypes{
    WS_CONFIG_2Server=0,
    WS_CLOSE_2Server,

    WS_CONFIG_2Client=1000,

    WS_FORWARD_2ServerClient=2000,
};
}

#endif // WEBSOCKETMSGTYPES_H
