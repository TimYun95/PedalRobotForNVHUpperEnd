#ifndef WEBSOCKETADAPTER_H
#define WEBSOCKETADAPTER_H

#include <string>

#include "communication/msgstructure.h"
#include "MyWebSocket/websocketcommunicator.h"

class JSONWrapper;

/* 针对WebSocketCommunicator的适配器 继承WebSocketCommunicator
 * 适配WebSocketCommunicator的SendWebSocketMsg()和CheckRecvMsg()接口
 * 发送:
 *   对于MsgStructure中的各种消息, 使用JSON序列化为JSON字符串后
 *   通过SendWebSocketMsg()交给WebSocketCommunicator进行加密发送
 * 接收:
 *   通过CheckRecvMsg()检查WebSocketCommunicator是否收到消息
 *   若收到消息, 则解析收到的JSON字符串(存放在recvJson中)
 *   之后, 程序通过ReceiveMsg()接收JSON字符串中的消息内容 */

class WebSocketAdapter: public WebSocketCommunicator
{
public:
    explicit WebSocketAdapter(const std::string& groupId, const std::string& groupPassword, const std::string& rsaPublicKeyFile);
    virtual ~WebSocketAdapter();

    /* 发送消息的接口 重载SendMsg() */
    int SendMsg(const MsgStructure::Configuration &cfg);
    int SendMsg(const MsgStructure::SingleAxis &sa);
    int SendMsg(const MsgStructure::InStatus &is);
    int SendMsg(const MsgStructure::StringStatus &ss);
    int SendMsg(const MsgStructure::SlowDown &sd);
    int SendMsg(const MsgStructure::ActionMessage &am);
    int SendMsg(const MsgStructure::OutStatus &os);
    int SendMsg(const MsgStructure::OutControl &oc);
    int SendMsg(const MsgStructure::ManualControl &mc);
    int SendMsg(const MsgStructure::GoHomeResult &ghr);
    int SendMsg(const MsgStructure::MatrixTheta &mt);
    int SendMsg(const MsgStructure::EditWeldSpeed &ews);
    int SendMsg(const MsgStructure::EditSpeed &es);
    int SendMsg(const MsgStructure::FileContent &fc);
    int SendMsg(const MsgStructure::MonitorCommand &mc);
    int SendMsg(const MsgStructure::PedalOpenValue &pov);
    int SendMsg(const MsgStructure::MedicalForceValue &mfv);
    int SendMsg(const MsgStructure::MessageInform &mi);
    int SendMsg(const int msgType);
private:
    /* 发送消息的辅助函数 */
    int SendJson(JSONWrapper& json, const MsgStructure::BaseStructure* msgStructure);
    int SendJsonStr(const std::string& jsonStr);

public:
    //return -1 if no message, return msgType if receive new message
    int CheckMsg();//检查所有消息

    /* 接收消息的接口 重载ReceiveMsg() */
    void ReceiveMsg(MsgStructure::Configuration &cfg);
    void ReceiveMsg(MsgStructure::SingleAxis &sa);
    void ReceiveMsg(MsgStructure::InStatus &is);
    void ReceiveMsg(MsgStructure::StringStatus &ss);
    void ReceiveMsg(MsgStructure::SlowDown &sd);
    void ReceiveMsg(MsgStructure::ActionMessage &am);
    void ReceiveMsg(MsgStructure::OutStatus &os);
    void ReceiveMsg(MsgStructure::OutControl &oc);
    void ReceiveMsg(MsgStructure::ManualControl &mc);
    void ReceiveMsg(MsgStructure::GoHomeResult &ghr);
    void ReceiveMsg(MsgStructure::MatrixTheta &mt);
    void ReceiveMsg(MsgStructure::EditWeldSpeed &ews);
    void ReceiveMsg(MsgStructure::EditSpeed &es);
    void ReceiveMsg(MsgStructure::FileContent &fc);
    void ReceiveMsg(MsgStructure::MonitorCommand &mc);
    void ReceiveMsg(MsgStructure::PedalOpenValue &pov);
    void ReceiveMsg(MsgStructure::MedicalForceValue &mfv);
    void ReceiveMsg(MsgStructure::MessageInform &mi);

private:
    JSONWrapper *recvJson;
    int jsonMsgType;
};

#endif // WEBSOCKETADAPTER_H
