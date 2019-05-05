#include "uriapibase.h"

#include "uri.h"
#include "unifiedrobot.h"

#include "common/printf.h"
#include "common/globalvariables.h"
#include "fileoperation/normalfile.h"

UriApiBase::UriApiBase(bool enableLogicUpdateTimer, bool enableWidgetUpdateTimer)
{
    m_uri = new URI(Q_NULLPTR, enableLogicUpdateTimer, enableWidgetUpdateTimer);
    m_conf = Configuration::Instance();

    InitUriApiBase();
}

UriApiBase::~UriApiBase()
{
    delete m_uri;
}

int UriApiBase::Api_ShowWidget(bool showFlag)
{
    if(showFlag){
        m_uri->show();
    }else{
        m_uri->hide();
    }
    return 0;
}

int UriApiBase::Api_GoHome(bool popupConfirmBox)
{
    Q_UNUSED(popupConfirmBox);
    UnifiedRobot::Instance()->SendGoHomeMsg();
    return 0;
}

int UriApiBase::Api_StopSingleAxis(const std::vector<int> &stopAxes)
{
    for(size_t i=0; i<stopAxes.size(); ++i){
        const int axisIndex = stopAxes.at(i);
        const double speed = 0.0;

        if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
            return -1;
        }

        UnifiedRobot::Instance()->SendSingleAxisMsg(axisIndex, speed);
    }
    return 0;
}

int UriApiBase::Api_MoveSingleAxis(const std::vector<int> &moveAxes, const std::vector<double> &moveSpeed)
{
    if(moveAxes.size() != moveSpeed.size()){
        return -1;
    }

    for(size_t i=0; i<moveAxes.size(); ++i){
        const int axisIndex = moveAxes.at(i);
        const double speed = moveSpeed.at(i);

        if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
            return -1;
        }

        UnifiedRobot::Instance()->SendSingleAxisMsg(axisIndex, speed);
    }
    return 0;
}

int UriApiBase::Api_SwitchToAction(const std::string &actionFileContent)
{
    const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + "/ApiActionFile.txt";
    int ret = NormalFile::WriteToFile(actionFilePath.c_str(), actionFileContent.c_str(), actionFileContent.length());
    if(ret == -1){
        return -1;
    }

    bool startOk = UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
    if(startOk){
        return 0;
    }else{
        return -1;
    }
}

int UriApiBase::Api_SetMonitorActionTheta(const std::vector<int> &actionMethod, const std::vector<int> &actionAxes, const std::vector<double> &actionTheta, int customVariable)
{
    bool sendOK = UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta, customVariable);
    if(sendOK){
        return 0;
    }else{
        return -1;
    }
}

int UriApiBase::Api_SwitchToIdleState()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
    return 0;
}

int UriApiBase::Api_SetVelocityActionSpeed(const std::vector<double> &actionSpeed)
{
    return UnifiedRobot::Instance()->SendVelocityCommandMsg(actionSpeed);
}

int UriApiBase::Api_SetSerialPort(int serialDeviceIndex)
{
    if(serialDeviceIndex < 0){
        return -1;
    }

    m_conf->serialDevice = serialDeviceIndex;

    return 0;
}

int UriApiBase::Api_SetPositionLimitConf(const std::vector<double> &positiveLimit, const std::vector<double> &negativeLimit)
{
    if((int)positiveLimit.size() != RobotParams::axisNum){
        return -1;
    }
    if((int)negativeLimit.size() != RobotParams::axisNum){
        return -1;
    }

    for(int i=0; i<RobotParams::axisNum; ++i){
        m_conf->limitPos[i] = positiveLimit[i];
        m_conf->limitNeg[i] = negativeLimit[i];
    }

    return 0;
}

int UriApiBase::Api_SetReservedParamConf(const std::vector<double> &reservedParam)
{
    int maxLength = RobotParams::reservedParamNum;
    int currentLength = reservedParam.size();
    int updateLength = std::min(maxLength, currentLength);

    for(int i=0; i<updateLength; ++i){
        m_conf->reservedParam[i] = reservedParam[i];
    }

    return 0;
}

int UriApiBase::Api_SaveAndSendConfMsg(bool saveFlag, bool sendFlag)
{
    if(saveFlag){
        m_conf->SaveToFile();
    }

    if(sendFlag){
        UnifiedRobot::Instance()->SendConfiguration();
    }

    return 0;
}

int UriApiBase::Api_MessageInformMsg(int informType, double informValue)
{
    bool sendOk = UnifiedRobot::Instance()->SendMessageInformMsg(informType, informValue);
    if(sendOk){
        return 0;
    }else{
        return -1;
    }
}

bool UriApiBase::Api_GetGoHomeStatus()
{
    return UnifiedRobot::Instance()->GetIsGoHomed();
}

bool UriApiBase::Api_GetIsEmergencyStopSuccess()
{
    return UnifiedRobot::Instance()->GetIsEmergencyStopSuccess();
}

std::vector<double> UriApiBase::Api_GetRobotTheta()
{
    std::vector<double> robotTheta(RobotParams::axisNum, 0.0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        robotTheta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
    }
    return robotTheta;
}

std::vector<double> UriApiBase::Api_GetRobotMatrix()
{
    std::vector<double> robotMatrix(RobotParams::dof, 0.0);
    for(int i=0; i<RobotParams::dof; ++i){
        robotMatrix[i] = RobotThetaMatrix::Instance()->GetMatrix(i);
    }
    return robotMatrix;
}

void UriApiBase::Api_GetUnifiedInform(std::vector<int> &intArray, std::vector<double> &doubleArray)
{
    const MsgStructure::UnifiedInform& unifiedInform = UnifiedRobot::Instance()->GetUnifiedInform();
    intArray = unifiedInform.intDataArray;
    doubleArray = unifiedInform.doubleDataArray;
}

std::string UriApiBase::Api_GetStatusString()
{
    std::string statusString = StatusString::Instance()->GetStatusString().toStdString();
    return statusString;
}

int UriApiBase::Api_GetStatusStringIndex()
{
    int statusStringIndex = StatusString::Instance()->GetStatusIndex();
    return statusStringIndex;
}

std::vector<double> UriApiBase::Api_GetPositionLimitConf(bool isPositive)
{
    std::vector<double> positionLimit(RobotParams::axisNum, 0.0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        positionLimit[i] = (isPositive ? m_conf->limitPos[i] : m_conf->limitNeg[i]);
    }
    return positionLimit;
}

bool UriApiBase::Api_InstallCallback(UriApiBase::eApiCallbackType apiCallbackType, UriApiBase::ApiCallbackFunc_t apiCallbackFunc)
{
    std::map<eApiCallbackType, ApiCallbackFunc_t>::iterator itr = m_apiCallbackType2Func.find(apiCallbackType);
    if(itr != m_apiCallbackType2Func.end()){
        return false;
    }

    m_apiCallbackType2Func.insert(std::make_pair(apiCallbackType, apiCallbackFunc));
    return true;
}

void UriApiBase::On_ReceiveMessageInform()
{
    //检查是否是急停的通知
    const MsgStructure::MessageInform& messageInform = UnifiedRobot::Instance()->GetMessageInform();
    switch (messageInform.informType) {
    case MsgStructure::MessageInform::EmergencyStop:
        break;
    case MsgStructure::MessageInform::EmergencyStopError:
        break;
    default:
        return;
    }

    DispatchCallback(eReceiveEmergencyStopResult);
}

void UriApiBase::On_ReceiveUnifiedInform()
{
    DispatchCallback(eReceiveUnifiedInform);
}

void UriApiBase::InitUriApiBase()
{
    UnifiedRobot::Instance()->RegisterActionCallback(Callback_UpdateMessageInform, std::tr1::bind(&UriApiBase::On_ReceiveMessageInform, this));
    UnifiedRobot::Instance()->RegisterActionCallback(Callback_UpdateUnifiedInform, std::tr1::bind(&UriApiBase::On_ReceiveUnifiedInform, this));
}

void UriApiBase::DispatchCallback(UriApiBase::eApiCallbackType apiCallbackType)
{
    std::map<eApiCallbackType, ApiCallbackFunc_t>::iterator itr = m_apiCallbackType2Func.find(apiCallbackType);
    if(itr != m_apiCallbackType2Func.end()){
        return;
    }
    ApiCallbackFunc_t& callbackFunc = itr->second;
    callbackFunc();
}
