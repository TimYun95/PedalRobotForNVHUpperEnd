#ifndef WEBSOCKET2MODBUS_H
#define WEBSOCKET2MODBUS_H

#include <tr1/functional>

class CommunicationMediator;

/* 限定使用于URI::OnMsg_WSMsgFromRemoteClient()
 *
 * 当本地UI接收远程UI发送的WebSocket消息时, 本地UI没有对应于这些消息的回调函数
 * (这部分消息最终应下行至Controller 由Robot::OnMsg_XXX()函数处理)
 *
 * 本地UI需要将这些消息从WebSocket协议中接收 然后通过Modbus协议发送给Controller
 * */

class WebSocket2Modbus
{
public:
    static WebSocket2Modbus* GetInstance();

    /* 根据msgType(见communication/msgtypes.h) 构造消息结构体MsgStructure
     * 1) 从WebSocket中ReceiveMsg
     * 2) 下行转发消息至Controller(在CommunicationMediator::ReceiveMsg()中自动完成)
     * */
    void ForwardMsg(const int msgType);

private:
    explicit WebSocket2Modbus();
    virtual ~WebSocket2Modbus();

    /* 该部分函数 和Robot.h中的声明相同(运行与ARM的Controller端) */
    void DummyMessageHandler(const int msgType);
    void OnMsg_Configuration(const int msgType);
    void OnMsg_SingleAxis(const int msgType);
    void OnMsg_StopPID(const int msgType);
    void OnMsg_SlowDown(const int msgType);
    void OnMsg_ActionStart(const int msgType);
    void OnMsg_FileContent(const int msgType);
    void OnMsg_OutControl(const int msgType);
    void OnMsg_ManualControl(const int msgType);
    void OnMsg_MessageInform(const int msgType);
    void OnMsg_MonitoringCommand(const int msgType);
    void OnMsg_ForceControl(const int msgType);
    void OnMsg_PedalOpenValue(const int msgType);

    void OnMsg_GoHome(const int msgType);
    void OnMsg_ExitWaitPoint(const int msgType);
    void OnMsg_ActionPause(const int msgType);
    void OnMsg_ActionResume(const int msgType);
    void OnMsg_EnterIdleState(const int msgType);
    void OnMsg_ShutDown(const int msgType);
    void OnMsg_Echo(const int msgType);


private:
    struct MessageHandler{
        typedef std::tr1::function<void (const int)> wsMsgHandler;
        wsMsgHandler handler;
    };

    MessageHandler msgHandler[100];
    CommunicationMediator* mediator;
};

#endif // WEBSOCKET2MODBUS_H
