#include "unifiedrobot.h"

#include "common/printf.h"
#include "communication/msgtypes.h"
#include "communicationmediator.h"
#include "util/timeutil.h"
#include "MyWebSocket/websocket2modbus.h"
#include "assistantfunc/fileassistantfunc.h"
#include "miscconfigurationparam.h"

static constexpr qint64 LostCmtConnectionIntervalMs = 10*1000;
static constexpr long EchoCmtConnectionIntervalMs = 3*1000;

UnifiedRobot::UnifiedRobot()
    : m_messageInform(MsgStructure::MessageInform::NullType, 0.0, MsgStructure::tagMsgUsage::Controller2UI)
{
    Configuration::CreateInstance();
    Configuration::Instance()->LoadDefaultConfiguration();
    Configuration::Instance()->ReadFromFile();
    RobotThetaMatrix::CreateInstance();
    StatusString::CreateInstance();
    MiscConfigurationParam::CreateInstance();
    CommunicationMediator::CreateInstance();

    m_conf = Configuration::Instance();
    m_rbtThetaMatrix = RobotThetaMatrix::Instance();
    m_cmtMediator = CommunicationMediator::Instance();

    m_sendMsgTimestampMs = 0;
    m_recvMsgTimestampMs = 0;
    m_isCmtConnected = false;

    m_isGoHomed = false;
    m_outCtrlStatus = 0;
    m_inSwitchStatus = 0;
    m_isEmergencyStopSuccess = false;
}

UnifiedRobot::~UnifiedRobot()
{
    CommunicationMediator::DestroyInstance();
    MiscConfigurationParam::DestroyInstance();
    StatusString::DestroyInstance();
    RobotThetaMatrix::DestroyInstance();
    Configuration::DestroyInstance();
}

void UnifiedRobot::Init()
{
    RegisterMessageHandler();
    SendConfiguration();
}

void UnifiedRobot::Uninit()
{

}

void UnifiedRobot::Update(qint64 currentTimestampMs)
{
    CheckMessage(currentTimestampMs);
    CheckCmtConnection(currentTimestampMs);

    long sendMsgIntervalMs = currentTimestampMs - m_sendMsgTimestampMs;
    if(sendMsgIntervalMs > EchoCmtConnectionIntervalMs){
        SendEchoMessage();
    }
}

int UnifiedRobot::GetStringStatusIndex()
{
    return StatusString::Instance()->GetStatusIndex();
}

bool UnifiedRobot::StartActionByTeachFile(const std::string &teachFilePath, short actionMsgUsage)
{
    if(!FileAssistantFunc::IsFileExist(teachFilePath)){
        PRINTF(LOG_ERR, "%s: not exist teachFile=%s\n", __func__, teachFilePath.c_str());
        return false;
    }

    SendTeachFile(teachFilePath);

    ResetActionMsg();
    UpdateActionMsgUsage(actionMsgUsage);
    SendActionMessage(m_actionMsg);

    return true;
}

bool UnifiedRobot::GetIsCmtConnected()
{
    return m_isCmtConnected;
}

bool UnifiedRobot::GetIsGoHomed()
{
    return m_isGoHomed;
}

int UnifiedRobot::GetGoHomeResult()
{
    return m_goHomeResult;
}

void UnifiedRobot::ResetGoHomeResult()
{
    m_goHomeResult = MsgStructure::GoHomeResult::Fail;
}

int UnifiedRobot::GetOutCtrlStatus()
{
    return m_outCtrlStatus;
}

int UnifiedRobot::GetInSwitchStatus()
{
    return m_inSwitchStatus;
}

bool UnifiedRobot::GetIsEmergencyStopSuccess()
{
    return m_isEmergencyStopSuccess;
}

const ActionMsg &UnifiedRobot::GetActionMsg()
{
    return m_actionMsg;
}

void UnifiedRobot::UpdateActionMsgUsage(short usage)
{
    m_actionMsg.usage = usage;
}

void UnifiedRobot::ResetActionMsg()
{
    m_actionMsg.beginSectionNo = 0;
    m_actionMsg.endSectionNo = 65535;
    m_actionMsg.pausedNo = 0;
    m_actionMsg.nowTimes = 0;
    m_actionMsg.loopTimes = 1;
    m_actionMsg.isCheck = false;

    m_actionMsg.moveToTranspositionSpeed = m_conf->transSpeed;
}

void UnifiedRobot::UpdateActionMsg(const ActionMsg &actionMsg)
{
    m_actionMsg = actionMsg;
}

const MsgStructure::MessageInform& UnifiedRobot::GetMessageInform()
{
    return m_messageInform;
}

const MsgStructure::UnifiedInform& UnifiedRobot::GetUnifiedInform()
{
    return m_unifiedInform;
}

void UnifiedRobot::RegisterActionCallback(eActionCallbackType actionCallbackType, ActionCallbackFunc_t callbackFunc)
{
    m_actionType2CallbackFuncList[actionCallbackType].push_back(callbackFunc);
}

void UnifiedRobot::DispatchActionCallback(eActionCallbackType actionCallbackType)
{
    std::map<eActionCallbackType, std::list<ActionCallbackFunc_t>>::iterator itrList = m_actionType2CallbackFuncList.find(actionCallbackType);
    if(itrList == m_actionType2CallbackFuncList.end()){
        PRINTF(LOG_ERR, "%s: type nout found, actionCallbackType=[%d]\n", __func__, actionCallbackType);
        return;
    }

    std::list<ActionCallbackFunc_t>& actionCallbackFuncList = itrList->second;

    for(std::list<ActionCallbackFunc_t>::iterator itrFunc = actionCallbackFuncList.begin(); itrFunc != actionCallbackFuncList.end(); ++itrFunc ){
        ActionCallbackFunc_t& actionCallbackFunc = *itrFunc;
        actionCallbackFunc();
    }
}

bool UnifiedRobot::SendEnterIdleMsg()
{
    m_cmtMediator->SendMsg(I2C_EnterIdleState, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendEchoMessage()
{
    m_cmtMediator->SendMsg(I2C_Echo, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendGoHomeMsg()
{
    ResetGoHomeResult();

    m_cmtMediator->SendMsg(I2C_GoHome, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendSingleAxisMsg(int axisIndex, double speed)
{
    MsgStructure::SingleAxis sa(axisIndex, speed);
    m_cmtMediator->SendMsg(sa, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendManualControlMsg(manual_method method, manual_direction direction, double speed)
{
    MsgStructure::ManualControl mc(method, direction, speed);
    m_cmtMediator->SendMsg(mc, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendOutControlCmd(int outStatus)
{
    MsgStructure::OutControl oc(outStatus);
    m_cmtMediator->SendMsg(oc, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendConfiguration()
{
    ConfigurationMsg confMsg;
    InitConfigurationMsg(&confMsg);

    MsgStructure::Configuration cfg(confMsg);
    m_cmtMediator->SendMsg(cfg, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendTeachFile(const std::string &filePath)
{
    std::string content;
    bool readOk = FileAssistantFunc::ReadFileContent(filePath, content);
    if(!readOk){
        PRINTF(LOG_DEBUG, "read error, fileName=[%s]\n", filePath.c_str());
        return false;
    }
    PRINTF(LOG_DEBUG, "read ok, fileName=[%s]\n", filePath.c_str());

    return SendTeachFileContent(content);
}

bool UnifiedRobot::SendTeachFileContent(const std::string &fileContent)
{
    MsgStructure::FileContent fc(fileContent);
    m_cmtMediator->SendMsg(fc, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendActionMessage(ActionMsg &actionMsg)
{
    MsgStructure::ActionMessage am(actionMsg, MsgStructure::UI2Controller);
    m_cmtMediator->SendMsg(am, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendActionPauseResumeMsg(bool isPausedFlag)
{
    int msgType = isPausedFlag ? I2C_ActionPause : I2C_ActionResume;
    m_cmtMediator->SendMsg(msgType, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendMessageInformMsg(int informType, double informValue)
{
    MsgStructure::MessageInform mi(informType, informValue, MsgStructure::UI2Controller);
    m_cmtMediator->SendMsg(mi, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendMonitorCommandMsg(const std::vector<int> &actionMethod, const std::vector<int> &actionAxes, const std::vector<double> &actionTheta, int customVariable)
{
    //actionAxes wait to configure to use
    if(actionMethod.size() != RobotParams::axisNum){
        return false;
    }else if(actionAxes.size() != RobotParams::axisNum){
        return false;
    }else if(actionTheta.size() != RobotParams::axisNum){
        return false;
    }

    int monitorMethod[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        monitorMethod[i] = actionMethod[i];
    }

    double monitorTheta[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        monitorTheta[i] = actionTheta[i];
    }

    MsgStructure::MonitorCommand mc(monitorMethod, monitorTheta, RobotParams::axisNum, customVariable);
    m_cmtMediator->SendMsg(mc, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

bool UnifiedRobot::SendVelocityCommandMsg(const std::vector<double> &actionSpeed)
{
    if(actionSpeed.size() != RobotParams::axisNum){
        return false;
    }

    double velocityTargetSpeed[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        velocityTargetSpeed[i] = actionSpeed[i];
    }

    MsgStructure::VelocityCommand vc(velocityTargetSpeed, RobotParams::axisNum);
    m_cmtMediator->SendMsg(vc, CommunicationMediator::DownlinkMsg);
    UpdateSendMsgTimestamp();
    return true;
}

void UnifiedRobot::UpdateSendMsgTimestamp()
{
    m_sendMsgTimestampMs = TimeUtil::CurrentTimestampMs();
}

void UnifiedRobot::InitConfigurationMsg(ConfigurationMsg *confMsg)
{
    confMsg->motorMaxPps = m_conf->motorMaxPps;
    confMsg->angleMutationThreshold = m_conf->angleMutationThreshold;
    confMsg->openGunTime = m_conf->openGunDelay;
    confMsg->closeGunTime = m_conf->closeGunDelay;
    confMsg->transPointRatio = m_conf->transPointRatio;

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->limitPos[i] = m_conf->limitPos[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->limitNeg[i] = m_conf->limitNeg[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->motorRatio[i] = m_conf->motorRatio[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->motorSubdivision[i] = m_conf->motorSubdivision[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->encoderRatio[i] = m_conf->encoderRatio[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->encoderSubdivision[i] = m_conf->encoderSubdivision[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->pulse_filter_limit[i] = m_conf->pulseFilter[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->speedLimit[i] = m_conf->speedLimit[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->motorAcceleration[i] = m_conf->motorAcceleration[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->maxMonitorDiffTheta[i] = m_conf->maxMonitorDiffTheta[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->kp[i] = m_conf->kp[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->ki[i] = m_conf->ki[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->kd[i] = m_conf->kd[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->kf[i] = m_conf->kf[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->kp_work[i] = m_conf->kp_work[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->ki_work[i] = m_conf->ki_work[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->kd_work[i] = m_conf->kd_work[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->kf_work[i] = m_conf->kf_work[i];

    confMsg->N1_7or5_ratio = m_conf->N1_7or5_ratio;
    confMsg->N2_7or5_ratio = m_conf->N2_7or5_ratio;
    confMsg->N4_7or5_ratio = m_conf->N4_7or5_ratio;
    confMsg->N6_7or5_ratio = m_conf->N6_7or5_ratio;
    confMsg->N7_7or5_ratio = m_conf->N7_7or5_ratio;

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->goHomeLimit[i] = m_conf->goHomeLimit[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->homeAngle[i] = m_conf->homeAngle[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->goHomeHighSpeed[i] = m_conf->goHomeHighSpeed[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->goHomeLowSpeed[i] = m_conf->goHomeLowSpeed[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->singleAbsValueAtOrigin[i] = m_conf->singleAbsValue[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->multiAbsValueAtOrigin[i] = m_conf->multiAbsValue[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->motorPort[i] = m_conf->motorPort[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->encoderPort[i] = m_conf->encoderPort[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->optSwitchPort[i] = m_conf->optSwitchPort[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->motorDirection[i] = m_conf->motorDirection[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->encoderDirection[i] = m_conf->encoderDirection[i];
    for(int i=0;i<RobotParams::axisNum;++i) confMsg->goHomeDirection[i] = m_conf->goHomeDirection[i];

    for(int i=0;i<RobotParams::axisNum;++i) confMsg->errorLimitTrans[i] = m_conf->errorLimitTrans[i];
    for(int i=0;i<RobotParams::dof;++i) confMsg->errorLimitWork[i] = m_conf->errorLimitWork[i];

    for(int i=0; i<RobotParams::machineParamNum; ++i) confMsg->machinePara[i] = m_conf->machineParam[i];
    for(int i=0; i<RobotParams::reservedParamNum; ++i) confMsg->reservedParam[i] = m_conf->reservedParam[i];

    InitConfigurationMsgByRobotType(confMsg);
}

void UnifiedRobot::InitConfigurationMsgByRobotType(ConfigurationMsg *confMsg)
{
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
    /* 踏板机器人利用machinePara传递一些配置 */
    //machineParam[0,1,2,3]传递配置 在SettingWidgetMachineParams::InitDifferentRobot()
    confMsg->machinePara[7] = m_conf->deathOpenValue[0];
    confMsg->machinePara[8] = m_conf->deathOpenValue[1];

    confMsg->machinePara[9] = m_conf->pedalPositionLimit[0];
    confMsg->machinePara[10] = m_conf->pedalPositionLimit[1];
#else
    Q_UNUSED(confMsg);
#endif
}

void UnifiedRobot::CheckMessage(qint64 currentTimestampMs)
{
    //Modbus message
    bool recvModbusMsgFlag = false;
    while(true){
        Message msg = m_cmtMediator->CheckMsg(CommunicationMediator::ModbusMsg);
        if(msg.IsNull()){//no new message
            break;
        }
        recvModbusMsgFlag = true;

        int msgType = msg.GetType();
        if(msgType>=0 && msgType<50){//0-49 上行消息 Modbus消息从Controller到本地UI
            m_msgHandlers[msgType].handler(msg, CommunicationMediator::ModbusMsg);
        }else{
            PRINTF(LOG_ERR, "%s: error msgType(%d), UI should never receive this kind of msg!\n", __func__, msgType);
        }
    }
    if(recvModbusMsgFlag){
        m_recvMsgTimestampMs = currentTimestampMs;
    }

#ifdef ENABLE_WEBSOCKET_COMMUNICATION
    //WebSocket message
    while(true){
        Message msg = m_cmtMediator->CheckMsg(CommunicationMediator::WebSocketMsg);
        if(msg.IsNull()){//no new message
            break;
        }

        int msgType = msg.GetType();
        if(msgType>=0 && msgType<50){//0-49 上行消息 WebSocket消息从本地UI到远程UI
            m_msgHandlers[msgType].handler(msg, CommunicationMediator::WebSocketMsg);
        }else{//50~99 下行消息 WebSocket消息从远程UI到本地UI
            //50~99的对应回调函数 应当均为 OnMsg_WSMsgFromRemoteClient
            m_msgHandlers[msgType].handler(msg, CommunicationMediator::WebSocketMsg);
        }
    }
#endif
}

void UnifiedRobot::CheckCmtConnection(qint64 currentTimestampMs)
{
    if(GetStringStatusIndex() == StatusString::Status_ConnectiongCommunication){
        return;
    }

    qint64 recvMsgIntervalMs = currentTimestampMs - m_recvMsgTimestampMs;
    if (recvMsgIntervalMs > LostCmtConnectionIntervalMs){
        if(m_isCmtConnected){
            m_isCmtConnected = false;
            PRINTF(LOG_DEBUG, "%s: lost connection\n", __func__);
        }
        StatusString::Instance()->SetStatusIndex(StatusString::Status_LostCommunication);
    }else{
        if(!m_isCmtConnected){
            m_isCmtConnected = true;
            PRINTF(LOG_DEBUG, "%s: establish connection\n", __func__);
        }
    }
}

void UnifiedRobot::RegisterMessageHandler()
{
    /*  register a handler to each specific message received by communicator,
     *  the handler will be called when the corresponding message arrived
     *  in ChessMessage() function */

    /* register 0~49 handlers to DummyMessageHandler */
    for(int i=0; i<50; i++){
        this->m_msgHandlers[i].handler             = std::tr1::bind(&UnifiedRobot::DummyMessageHandler, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    }

    /* register 50~99 handlers to OnMsg_WSMsgFromRemoteClient */
    for(int i=50; i<100; i++){
        this->m_msgHandlers[i].handler             = std::tr1::bind(&UnifiedRobot::OnMsg_WSMsgFromRemoteClient, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    }

    /* register handlers needed */
    this->m_msgHandlers[C2I_MatrixTheta].handler   = std::tr1::bind(&UnifiedRobot::OnMsg_MatrixTheta, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_FinishGoHome].handler  = std::tr1::bind(&UnifiedRobot::OnMsg_FinishGoHome, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_StringStatus].handler  = std::tr1::bind(&UnifiedRobot::OnMsg_StringStatus, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_FileLength].handler    = std::tr1::bind(&UnifiedRobot::OnMsg_FileLength, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_StopWeld].handler      = std::tr1::bind(&UnifiedRobot::OnMsg_StopWeld, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_ActionStatus].handler  = std::tr1::bind(&UnifiedRobot::OnMsg_ActionStatus, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_OutStatus].handler     = std::tr1::bind(&UnifiedRobot::OnMsg_OutStatus, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_InStatus].handler      = std::tr1::bind(&UnifiedRobot::OnMsg_InStatus, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);

    this->m_msgHandlers[C2I_MessageInform].handler = std::tr1::bind(&UnifiedRobot::OnMsg_MessageInform, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_UnifiedInform].handler = std::tr1::bind(&UnifiedRobot::OnMsg_UnifiedInform, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_FinishAction].handler  = std::tr1::bind(&UnifiedRobot::OnMsg_FinishAction, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_AtSuspendPoint].handler= std::tr1::bind(&UnifiedRobot::OnMsg_AtSuspendPoint, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_ReInit].handler        = std::tr1::bind(&UnifiedRobot::OnMsg_ReInit, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
    this->m_msgHandlers[C2I_EnterIdleState].handler= std::tr1::bind(&UnifiedRobot::OnMsg_EnterIdleState, this, std::tr1::placeholders::_1, std::tr1::placeholders::_2);
}

void UnifiedRobot::DummyMessageHandler(const Message& msg, const int recvMsgType)
{
    PRINTF(LOG_NOTICE, "%s: dummy msg(type=%d, from=%s)\n",
           __func__, msg.GetType(), recvMsgType==CommunicationMediator::ModbusMsg? "ModbusMsg": "WebSocketMsg");
}

void UnifiedRobot::OnMsg_MatrixTheta(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);

    double robotTheta[RobotParams::axisNum];
    double robotMatrix[RobotParams::dof];
    MsgStructure::MatrixTheta mt(robotTheta, RobotParams::axisNum, robotMatrix, RobotParams::dof);
    m_cmtMediator->ReceiveMsg(mt, recvMsgType);

    for(int i=0; i<RobotParams::axisNum; ++i){
        m_rbtThetaMatrix->SetTheta(i, robotTheta[i]);
    }
    for(int i=0; i<RobotParams::dof; ++i){
        m_rbtThetaMatrix->SetMatrix(i, robotMatrix[i]);
    }
}

void UnifiedRobot::OnMsg_FinishGoHome(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    MsgStructure::GoHomeResult ghr;
    m_cmtMediator->ReceiveMsg(ghr, recvMsgType);

    m_goHomeResult = ghr.ghResult;
    Logic_ProcessGoHomeResult(m_goHomeResult);

    DispatchActionCallback(Callback_UpdateGoHomeResult);
}

void UnifiedRobot::OnMsg_StringStatus(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);

    MsgStructure::StringStatus ss;
    m_cmtMediator->ReceiveMsg(ss, recvMsgType);

    int statusIndex = ss.strStatus;
    StatusString::Instance()->SetStatusIndex(statusIndex);
}

void UnifiedRobot::OnMsg_FileLength(const Message &msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    Q_UNUSED(recvMsgType);
    PRINTF(LOG_DEBUG, "%s...\n", __func__);
}

void UnifiedRobot::OnMsg_StopWeld(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    Q_UNUSED(recvMsgType);
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    DispatchActionCallback(Callback_UpdateStopWeld);
}

void UnifiedRobot::OnMsg_ActionStatus(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);

    MsgStructure::ActionMessage am(m_actionMsg, MsgStructure::Controller2UI);
    m_cmtMediator->ReceiveMsg(am, recvMsgType);
}

void UnifiedRobot::OnMsg_OutStatus(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);

    MsgStructure::OutStatus os;
    m_cmtMediator->ReceiveMsg(os, recvMsgType);

    m_outCtrlStatus = os.outStatus;

    DispatchActionCallback(Callback_UpdateOutControlStatus);
}

void UnifiedRobot::OnMsg_InStatus(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);

    MsgStructure::InStatus is;
    m_cmtMediator->ReceiveMsg(is, recvMsgType);

    m_inSwitchStatus = is.inStatus;

    DispatchActionCallback(Callback_UpdateInSwitchStatus);
}

void UnifiedRobot::OnMsg_MessageInform(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    MsgStructure::MessageInform mi(MsgStructure::MessageInform::NullType, 0, MsgStructure::Controller2UI);
    m_cmtMediator->ReceiveMsg(mi, recvMsgType);

    m_messageInform = mi;

    Logic_ProcessMessageInform();
    DispatchActionCallback(Callback_UpdateMessageInform);
}

void UnifiedRobot::OnMsg_UnifiedInform(const Message &msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    MsgStructure::UnifiedInform ui;
    m_cmtMediator->ReceiveMsg(ui, recvMsgType);

    m_unifiedInform = ui;

    Logic_ProcessUnifiedInform();
    DispatchActionCallback(Callback_UpdateUnifiedInform);
}

void UnifiedRobot::OnMsg_ReInit(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    Q_UNUSED(recvMsgType);
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    DispatchActionCallback(Callback_UpdateReInit);
}

void UnifiedRobot::OnMsg_EnterIdleState(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    Q_UNUSED(recvMsgType);
}

void UnifiedRobot::OnMsg_FinishAction(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    Q_UNUSED(recvMsgType);

    DispatchActionCallback(Callback_UpdateFinishAction);
}

void UnifiedRobot::OnMsg_AtSuspendPoint(const Message& msg, const int recvMsgType)
{
    Q_UNUSED(msg);
    Q_UNUSED(recvMsgType);

    DispatchActionCallback(Callback_UpdateAtSuspendPoint);
}

void UnifiedRobot::OnMsg_WSMsgFromRemoteClient(const Message &msg, const int recvMsgType)
{
    /* 应当只有本地UI会接收该种消息 且 消息类型应当为WebSocketMsg
     * 本地UI通过Modbus将这些消息转发给Controller
     * */
    assert(recvMsgType == CommunicationMediator::WebSocketMsg);

    WebSocket2Modbus::GetInstance()->ForwardMsg( msg.GetType() );
}

void UnifiedRobot::Logic_ProcessGoHomeResult(int goHomeResult)
{
    switch (goHomeResult) {
    case MsgStructure::GoHomeResult::Fail:
        m_isGoHomed = false;
        break;
    case MsgStructure::GoHomeResult::Success:
        m_isGoHomed = true;
        break;
    case MsgStructure::GoHomeResult::RequestTeachFile:
        m_isGoHomed = false;

        PRINTF(LOG_DEBUG, "%s: RequestTeachFile\n", __func__);
        StartActionByTeachFile(m_conf->GetFilePath(Configuration::RobotOriginFilePath), ActionMsg::MoveAfterGoHome);
        break;
    case MsgStructure::GoHomeResult::MovingToTargetPoint:
        m_isGoHomed = false;
        break;
    case MsgStructure::GoHomeResult::MovingToTargetPointSuccessfully:
        m_isGoHomed = true;
        break;
    case MsgStructure::GoHomeResult::MovingToTargetPointUnsuccessfully:
        m_isGoHomed = false;
        break;
    default:
        PRINTF(LOG_INFO, "%s: undefined result=[%d]\n", __func__, goHomeResult);
        break;
    }
}

void UnifiedRobot::Logic_ProcessMessageInform()
{
    const int type = m_messageInform.informType;
    const double content = m_messageInform.informContent;
    switch (type) {
    case MsgStructure::MessageInform::MedicalAxis4Angle:
        m_rbtThetaMatrix->SetTheta(3, content);
        break;
    case MsgStructure::MessageInform::EmergencyStop:
        m_isEmergencyStopSuccess = true;
        break;
    case MsgStructure::MessageInform::EmergencyStopError:
        m_isEmergencyStopSuccess = false;
        break;
    default:
        PRINTF(LOG_INFO, "%s: undefined inform type(%d)\n",__func__,type);
        break;
    }
}

void UnifiedRobot::Logic_ProcessUnifiedInform()
{

}
