#include "pedalrobotuiapi.h"

#include <QFile>

#include "common/globalvariables.h"
#include "common/printf.h"
#include "common/robotparams.h"

#include "uri.h"
#include "robottype/pedalrobot/pedalrobot.h"
#include "communication/msgstructure.h"
#include "communicationmediator.h"

PedalRobotUiApi::PedalRobotUiApi()
{
    m_pedalRobotUi = new PedalRobotUI();
    m_pedalRobot = m_pedalRobotUi->GetPedalRobot();

    m_monitorControlTheta.resize(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        m_monitorControlTheta[i] = 0.0;
    }
    m_monitorControlMethod = MsgStructure::MonitorCommand::AbsControl;
#ifdef DOF_6_PEDAL_SHIFTGEAR_SWERVE
    m_enablePeriodicallySendMonitorCommand = false;
#else
    m_enablePeriodicallySendMonitorCommand = true;
#endif

    m_pedalRobot->RegisterCallback_OnLogicControl(std::tr1::bind(PedalRobotUiApi::OnLogicControlCallbackFunc, this));
}

PedalRobotUiApi::~PedalRobotUiApi()
{
    delete m_pedalRobotUi;
}

void PedalRobotUiApi::ShowWidget(bool flag)
{
    if(flag){
        m_pedalRobotUi->show();
    }else{
        m_pedalRobotUi->hide();
    }
}

double PedalRobotUiApi::GetCanOpenValue(PedalRobotUiApi::ePedalNum pedal)
{
    switch(pedal){
    case eBrakePedal:
        return m_pedalRobot->GetBrakePosition();
    case eAccPedal:
        return m_pedalRobot->GetAcceleratorPosition();
    default:
        PRINTF(LOG_ERR, "%s: invalid pedal(%d).\n", __func__, pedal);
        return -1.0;
    }
}

double PedalRobotUiApi::GetCanCarSpeed()
{
    return m_pedalRobot->GetCanCarSpeed();
}

int PedalRobotUiApi::GetCanPowermode()
{
    return m_pedalRobot->GetPowerMode();
}

std::vector<double> PedalRobotUiApi::GetCanDataAll()
{
    return m_pedalRobot->GetCanAnalystData();
}

void PedalRobotUiApi::SetCanDataAll(const std::vector<double> &canData)
{
    m_pedalRobot->SetCanAnalystData(canData);
}

void PedalRobotUiApi::SetEmergencyStopTheta(int emergencyStopType, const std::vector<double> &emergencyStopTheta)
{
    if(emergencyStopTheta.size() != RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: error emergencyStopTheta size!\n", __func__);
        return;
    }

    Configuration *conf = Configuration::Instance();
    conf->machineParam[11] = emergencyStopType;
    for(int i=0; i<RobotParams::axisNum; ++i){
        conf->machineParam[12+i] = emergencyStopTheta[i];
    }
    conf->SaveToFile();
}

double PedalRobotUiApi::GetPulseCarSpeed()
{
    return m_pedalRobot->GetMP412CarSpeed();
}

void PedalRobotUiApi::DoGoHome()
{
    m_pedalRobot->StopLogicControl();

    m_pedalRobot->ResetOriginFileByDeathValue();
    UnifiedRobot::Instance()->SendGoHomeMsg();
}

PedalRobotUiApi::eGoHomeResultType PedalRobotUiApi::GetGoHomeResult()
{
    int goHomeResult = UnifiedRobot::Instance()->GetGoHomeResult();
    switch (goHomeResult) {
    case MsgStructure::GoHomeResult::Fail:
        return eGoHomeFail;
    case MsgStructure::GoHomeResult::Success:
        return eGoHomeOk;
    case MsgStructure::GoHomeResult::RequestTeachFile:
        return eGoHomeRunning;
    case MsgStructure::GoHomeResult::MovingToTargetPoint:
        return eGoHomeRunning;
    case MsgStructure::GoHomeResult::MovingToTargetPointSuccessfully:
        return eGoHomeOk;
    case MsgStructure::GoHomeResult::MovingToTargetPointUnsuccessfully:
        return eGoHomeFail;
    default:
        PRINTF(LOG_INFO, "%s: undefined result=[%d]\n", __func__, goHomeResult);
        return eGoHomeFail;
    }
}

double PedalRobotUiApi::GetAbsolutePosition(PedalRobotUiApi::ePedalNum pedal)
{
    int axisIndex = 0;
    switch(pedal){
    case eBrakePedal:
        axisIndex = 0;
        break;
    case eAccPedal:
        axisIndex = 1;
        break;
    default:
        PRINTF(LOG_ERR, "%s: invalid pedal(%d).\n", __func__, pedal);
        axisIndex = 0;
        break;
    }

    return RobotThetaMatrix::Instance()->GetTheta(axisIndex);
}

std::string PedalRobotUiApi::GetStatusString()
{
    return StatusString::Instance()->GetStatusString().toStdString();
}

bool PedalRobotUiApi::EnterIntoMonitorControlMode(const std::string &actionFilePath, const std::string &curveFilePath)
{
    if(!QFile::exists(actionFilePath.c_str())){
        PRINTF(LOG_ERR, "%s: not exits actionFilePath=[%s]!\n", __func__, actionFilePath.c_str());
        return false;
    }
    if(!QFile::exists(curveFilePath.c_str())){
        PRINTF(LOG_ERR, "%s: not exits curveFilePath=[%s]!\n", __func__, curveFilePath.c_str());
        return false;
    }

    bool stopOk = UnifiedRobot::Instance()->SendEnterIdleMsg();
    if(!stopOk){
        return false;
    }

    double robotTheta[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        robotTheta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
    }
    for(int i=0; i<RobotParams::axisNum; ++i){
        m_monitorControlTheta[i] = robotTheta[i];
    }
    m_monitorControlMethod = MsgStructure::MonitorCommand::AbsControl;

    bool startActionOk = UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
    if(!startActionOk){
        return false;
    }

    bool startPlotOk = m_pedalRobot->StartQCustomPlot(curveFilePath);
    if(!startPlotOk){
        return false;
    }
    m_pedalRobot->StartLogicControl();

    return true;
}

bool PedalRobotUiApi::SetMonitorControlModePosition(PedalRobotUiApi::ePedalNum pedal, double position_mm, PedalRobotUiApi::eControlMethodType method)
{
    int axisIndex = 0;
    switch(pedal){
    case eBrakePedal:
        axisIndex = 0;
        break;
    case eAccPedal:
        axisIndex = 1;
        break;
    default:
        PRINTF(LOG_ERR, "%s: invalid pedal(%d).\n", __func__, pedal);
        return false;
    }
    //设定即可 后续定时器周期发送设定的目标位置
    m_monitorControlTheta[axisIndex] = position_mm;

    switch(method){
    case eAbsolutePosition:
        m_monitorControlMethod = MsgStructure::MonitorCommand::AbsControl;
        break;
    case eDeltaPosition:
        m_monitorControlMethod = MsgStructure::MonitorCommand::DeltaControl;
        break;
    default:
        PRINTF(LOG_ERR, "%s: invalid mtehod(%d).\n", __func__, method);
        return false;
    }

    return true;
}

void PedalRobotUiApi::StopRobot()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    m_pedalRobot->StopLogicControl();
    m_pedalRobot->FinishQCustomPlot();
}

void PedalRobotUiApi::SendMonitorCommand()
{
    std::vector<double> actionTheta(RobotParams::axisNum, 0.0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionTheta[i] = m_monitorControlTheta[i];
    }

    std::vector<int> actionAxes(RobotParams::axisNum, 0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionAxes[i] = i;
    }

    std::vector<int> actionMethod(RobotParams::axisNum, 0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionMethod[i] = m_monitorControlMethod;
    }

    UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
}

void PedalRobotUiApi::OnLogicControlCallbackFunc()
{
    if(m_enablePeriodicallySendMonitorCommand){
        SendMonitorCommand();
    }
}
