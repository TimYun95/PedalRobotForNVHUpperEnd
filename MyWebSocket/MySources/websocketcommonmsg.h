#ifndef WEBSOCKETCOMMONMSG_H
#define WEBSOCKETCOMMONMSG_H

/* WebSocket中涉及的大型消息结构体的JSON数据序列化
 * 主要处理common/message.h中的 ActionMsg/ConfigurationMsg/EditSpeedMsg 这三个消息的序列化 */

#include "communication/msgstructure.h"

#include "MyWebSocket/MySources/JsonWrapper/jsonwrapper.h"

class WebSocketCommonMsg
{
public:
    //ActionMsg
    static void SerializeActMsg(const MsgStructure::ActionMessage& action, JSONWrapper& json);
    static void DeserializeActMsg(JSONWrapper &json, MsgStructure::ActionMessage& action);

    //ConfigurationMsg
    static void SerializeConfMsg(const MsgStructure::Configuration& conf, JSONWrapper& json);
    static void DeserializeConfMsg(JSONWrapper &json, MsgStructure::Configuration& conf);

    //EditSpeedMsg
    static void SerializeEditSpeedMsg(const MsgStructure::EditSpeed& edit, JSONWrapper& json);
    static void DeserializeEditSpeedMsg(JSONWrapper& json, MsgStructure::EditSpeed& edit);

private:
    WebSocketCommonMsg() = delete;
};

#endif // WEBSOCKETCOMMONMSG_H
