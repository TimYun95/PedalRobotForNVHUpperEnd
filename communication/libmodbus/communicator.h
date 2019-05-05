#ifndef COMMUNICATOR_H
#define COMMUNICATOR_H

#include "modbusqueue.h"
#include "communication/msgstructure.h"

class EndianConverter;

class Communicator{
public:
    Communicator(const int id, const char* comNum);
    enum {
        UI = ModbusQueue::Master,
        Controller = ModbusQueue::Slaver
    };
    virtual ~Communicator();

    /* 发送消息的接口 重载SendMsg() */
    /**********************派生类 带消息内容***********************/
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
    int SendMsg(const MsgStructure::UnifiedInform &ui);
    int SendMsg(const MsgStructure::VelocityCommand &vc);
    /**************************无消息内容**************************/
    int SendMsg(const int msgType);

private:
    /* 辅助函数 将Endianonverter压入modbus发送的队列中 */
    int SendMsgEndianConverter(int msgType, EndianConverter *ec);

public:
    Message CheckMsg();//检查所有消息
    /* 接收消息的接口 重载ReceiveMsg() */
    /**********************派生类 带消息内容***********************/
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
    void ReceiveMsg(MsgStructure::UnifiedInform &ui);
    void ReceiveMsg(MsgStructure::VelocityCommand &vc);

    /* 拒绝拷贝构造/赋值函数 */
private:
    Communicator(const Communicator&);
    Communicator& operator=(const Communicator&);

private:
    ModbusQueue *modbus;
};
#endif
