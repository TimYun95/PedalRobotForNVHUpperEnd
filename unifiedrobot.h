#ifndef UNIFIEDROBOT_H
#define UNIFIEDROBOT_H

#include <list>
#include "template/singleton.h"

#include "configuration.h"
#include "actioncallbacktype.h"

#include "communication/messagehandler.h"
#include "common/globalvariables.h"
#include "communicationmediator.h"

typedef std::tr1::function<void (void)> ActionCallbackFunc_t;

class UnifiedRobot : public Singleton<UnifiedRobot>
{
public:
    explicit UnifiedRobot();
    virtual ~UnifiedRobot();

    void Init();
    void Uninit();
    void Update(qint64 currentTimestampMs);

    int GetStringStatusIndex();

    bool StartActionByTeachFile(const std::string& teachFilePath, short actionMsgUsage=ActionMsg::NormalTeach);

public:
    bool GetIsCmtConnected();
    bool GetIsGoHomed();
    int GetGoHomeResult();
    void ResetGoHomeResult();
    int GetOutCtrlStatus();
    int GetInSwitchStatus();
    bool GetIsEmergencyStopSuccess();

    const ActionMsg& GetActionMsg();
    void UpdateActionMsgUsage(short usage);
    void ResetActionMsg();
    void UpdateActionMsg(const ActionMsg& actionMsg);

    const MsgStructure::MessageInform& GetMessageInform();
    const MsgStructure::UnifiedInform& GetUnifiedInform();

public:
    void RegisterActionCallback(eActionCallbackType actionCallbackType, ActionCallbackFunc_t callbackFunc);
    void DispatchActionCallback(eActionCallbackType actionCallbackType);

    //Communication
public:
    bool SendEnterIdleMsg();
    bool SendEchoMessage();
    bool SendGoHomeMsg();
    bool SendSingleAxisMsg(int axisIndex, double speed);
    bool SendManualControlMsg(manual_method method, manual_direction direction, double speed);
    bool SendOutControlCmd(int outStatus);
    bool SendConfiguration();
    bool SendTeachFile(const std::string& filePath);
    bool SendTeachFileContent(const std::string& fileContent);
    bool SendActionMessage(ActionMsg& actionMsg);
    bool SendActionPauseResumeMsg(bool isPausedFlag);
    bool SendMessageInformMsg(int informType, double informValue=0.0);
    bool SendMonitorCommandMsg(const std::vector<int> &actionMethod, const std::vector<int> &actionAxes, const std::vector<double> &actionTheta, int customVariable=0);
    bool SendVelocityCommandMsg(const std::vector<double> &actionSpeed);

protected:
    void UpdateSendMsgTimestamp();
    void InitConfigurationMsg(ConfigurationMsg* confMsg);
    void InitConfigurationMsgByRobotType(ConfigurationMsg* confMsg);

protected:
    void CheckMessage(qint64 currentTimestampMs);
    void CheckCmtConnection(qint64 currentTimestampMs);

    void RegisterMessageHandler();
    void DummyMessageHandler(const Message& msg, const int recvMsgType);
    void OnMsg_MatrixTheta(const Message& msg, const int recvMsgType);
    void OnMsg_FinishGoHome(const Message& msg, const int recvMsgType);
    void OnMsg_StringStatus(const Message& msg, const int recvMsgType);
    void OnMsg_FileLength(const Message& msg, const int recvMsgType);
    void OnMsg_StopWeld(const Message& msg, const int recvMsgType);
    void OnMsg_ActionStatus(const Message& msg, const int recvMsgType);
    void OnMsg_OutStatus(const Message& msg, const int recvMsgType);
    void OnMsg_InStatus(const Message& msg, const int recvMsgType);
    void OnMsg_MessageInform(const Message& msg, const int recvMsgType);
    void OnMsg_UnifiedInform(const Message& msg, const int recvMsgType);
    void OnMsg_ReInit(const Message& msg, const int recvMsgType);
    void OnMsg_EnterIdleState(const Message& msg, const int recvMsgType);
    void OnMsg_FinishAction(const Message& msg, const int recvMsgType);
    void OnMsg_AtSuspendPoint(const Message& msg, const int recvMsgType);
    void OnMsg_WSMsgFromRemoteClient(const Message& msg, const int recvMsgType);

protected:
    void Logic_ProcessGoHomeResult(int goHomeResult);
    void Logic_ProcessMessageInform();
    void Logic_ProcessUnifiedInform();

private:
    Configuration* m_conf;
    RobotThetaMatrix* m_rbtThetaMatrix;
    CommunicationMediator* m_cmtMediator;

    std::map<eActionCallbackType, std::list<ActionCallbackFunc_t>> m_actionType2CallbackFuncList;

    qint64 m_sendMsgTimestampMs;
    qint64 m_recvMsgTimestampMs;
    bool m_isCmtConnected;
    MessageHandler m_msgHandlers[100];

    bool m_isGoHomed;//是否回原
    int m_goHomeResult;//回原上报的结果
    int m_outCtrlStatus;//输出IO的控制状态
    int m_inSwitchStatus;//输入IO的开关状态
    bool m_isEmergencyStopSuccess;//急停是否成功
    ActionMsg m_actionMsg;//Action状态
    MsgStructure::MessageInform m_messageInform;
    MsgStructure::UnifiedInform m_unifiedInform;
};

#endif // UNIFIEDROBOT_H
