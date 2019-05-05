#include "websocket2modbus.h"

#include <cassert>

#include "common/printf.h"
#include "communication/msgtypes.h"
#include "communication/msgstructure.h"

#include "communicationmediator.h"

WebSocket2Modbus *WebSocket2Modbus::GetInstance()
{
    static WebSocket2Modbus* instance = NULL;
    if(instance == NULL){
        instance = new WebSocket2Modbus();
    }

    return instance;
}

void WebSocket2Modbus::ForwardMsg(const int msgType)
{
    //接收到的应当是msgType=50~99 远程UI的消息由本地UI转发给Controller
    assert(50<=msgType);
    assert(msgType<100);

    msgHandler[msgType].handler(msgType);
}

WebSocket2Modbus::WebSocket2Modbus()
    : mediator(NULL)
{
    /* 这部分代码与Robot::RegisterMessageHandler()应当是一致的 */

    /* register all handlers to DummyMessageHandler */
    for(size_t i=0; i<sizeof(msgHandler)/sizeof(msgHandler[0]); ++i){
        msgHandler[i].handler = std::tr1::bind(&WebSocket2Modbus::DummyMessageHandler, this, std::tr1::placeholders::_1);
    }

    /* register handlers needed */
    this->msgHandler[I2C_Configuration].handler    = std::tr1::bind(&WebSocket2Modbus::OnMsg_Configuration,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SingleAxisSpeed0].handler = std::tr1::bind(&WebSocket2Modbus::OnMsg_SingleAxis,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SingleAxisSpeed1].handler = std::tr1::bind(&WebSocket2Modbus::OnMsg_SingleAxis,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SingleAxisSpeed2].handler = std::tr1::bind(&WebSocket2Modbus::OnMsg_SingleAxis,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SingleAxisSpeed3].handler = std::tr1::bind(&WebSocket2Modbus::OnMsg_SingleAxis,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SingleAxisSpeed4].handler = std::tr1::bind(&WebSocket2Modbus::OnMsg_SingleAxis,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SingleAxisSpeed5].handler = std::tr1::bind(&WebSocket2Modbus::OnMsg_SingleAxis,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_StopPid].handler          = std::tr1::bind(&WebSocket2Modbus::OnMsg_StopPID,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_SlowDown].handler         = std::tr1::bind(&WebSocket2Modbus::OnMsg_SlowDown,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_ActionStart].handler      = std::tr1::bind(&WebSocket2Modbus::OnMsg_ActionStart,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_FileContent].handler      = std::tr1::bind(&WebSocket2Modbus::OnMsg_FileContent,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_OutControl].handler       = std::tr1::bind(&WebSocket2Modbus::OnMsg_OutControl,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_ManualControl].handler    = std::tr1::bind(&WebSocket2Modbus::OnMsg_ManualControl,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_Echo].handler             = std::tr1::bind(&WebSocket2Modbus::OnMsg_Echo,this,std::tr1::placeholders::_1);

    this->msgHandler[I2C_EnterIdleState].handler   = std::tr1::bind(&WebSocket2Modbus::OnMsg_EnterIdleState,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_GoHome].handler           = std::tr1::bind(&WebSocket2Modbus::OnMsg_GoHome,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_ExitWaitPoint].handler    = std::tr1::bind(&WebSocket2Modbus::OnMsg_ExitWaitPoint,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_ActionPause].handler      = std::tr1::bind(&WebSocket2Modbus::OnMsg_ActionPause,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_ActionResume].handler     = std::tr1::bind(&WebSocket2Modbus::OnMsg_ActionResume,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_Shutdown].handler         = std::tr1::bind(&WebSocket2Modbus::OnMsg_ShutDown,this,std::tr1::placeholders::_1);
    this->msgHandler[I2C_MessageInform].handler    = std::tr1::bind(&WebSocket2Modbus::OnMsg_MessageInform,this,std::tr1::placeholders::_1);

    mediator = CommunicationMediator::Instance();
}

WebSocket2Modbus::~WebSocket2Modbus()
{

}

void WebSocket2Modbus::DummyMessageHandler(const int msgType)
{
    Q_UNUSED(msgType);
    PRINTF(LOG_ERR, "WebSocket2Modbus::%s: error msg(type=%d), this class is not designed for this msg!\n", __func__, msgType);
}

void WebSocket2Modbus::OnMsg_Configuration(const int msgType)
{
    Q_UNUSED(msgType);

    ConfigurationMsg confMsg;
    MsgStructure::Configuration cfg(confMsg);
    mediator->ReceiveMsg(cfg, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_SingleAxis(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::SingleAxis sa;
    mediator->ReceiveMsg(sa, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_StopPID(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_SlowDown(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::SlowDown sd;
    mediator->ReceiveMsg(sd, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_ActionStart(const int msgType)
{
    Q_UNUSED(msgType);

    ActionMsg myActionMsg;
    MsgStructure::ActionMessage am(myActionMsg, MsgStructure::UI2Controller);
    mediator->ReceiveMsg(am, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_FileContent(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::FileContent fc("nop");
    mediator->ReceiveMsg(fc, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_OutControl(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::OutControl oc;
    mediator->ReceiveMsg(oc, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_ManualControl(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::ManualControl mc;
    mediator->ReceiveMsg(mc, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_MessageInform(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::MessageInform mi(MsgStructure::MessageInform::NullType, 0.0, MsgStructure::UI2Controller);
    mediator->ReceiveMsg(mi, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_MonitoringCommand(const int msgType)
{
    Q_UNUSED(msgType);

    int monitorMethod[RobotParams::axisNum]={0};
    double monitorTheta[RobotParams::axisNum]={0.0};
    MsgStructure::MonitorCommand mc(monitorMethod, monitorTheta, RobotParams::axisNum, 0);
    mediator->ReceiveMsg(mc, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_ForceControl(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::MedicalForceValue mfv;
    mediator->ReceiveMsg(mfv, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_PedalOpenValue(const int msgType)
{
    Q_UNUSED(msgType);

    MsgStructure::PedalOpenValue pov;
    mediator->ReceiveMsg(pov, CommunicationMediator::WebSocketMsg);
}

void WebSocket2Modbus::OnMsg_GoHome(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_ExitWaitPoint(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_ActionPause(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_ActionResume(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_EnterIdleState(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_ShutDown(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}

void WebSocket2Modbus::OnMsg_Echo(const int msgType)
{
    mediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
}
