#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "RobotAPI.h"

#include "uri.h"
#include "configuration.h"
#include "common/printf.h"
#include "common/globalvariables.h"

#include <QStatusBar>
#include <QVBoxLayout>
#include <QHBoxLayout>

RobotAPI::RobotAPI(QWidget *parent) :
    QWidget(parent)
{
    this->pUri=new URI(this);
    QObject::connect(pUri, SIGNAL(Signal_MedicalUltrasoundRobotAxis4Theta()), this, SLOT(ReceiveAxis4AngleSlot()));
}

RobotAPI::~RobotAPI()
{
    delete pUri;
}

void RobotAPI::ReceiveAxis4AngleSlot()
{
    emit FastControlOKSignal();
}

int RobotAPI::SetAbsoluteAngle(int axisNum, double angle, double speed)
{
    if(speed < 0 || speed > 100){
        PRINTF(LOG_ERR, "%s: invalid speed.\n",__func__);
        return -1;
    }
    if(axisNum < 0 || axisNum >3){
        PRINTF(LOG_ERR, "%s: invalid axisNum.\n",__func__);
        return -1;
    }

    std::string filePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + "SingleAxisLocateMove.txt";
    Teach::ConstructAxisLocateMoveTeachFile(axisNum, angle, speed, filePath);
    UnifiedRobot::Instance()->StartActionByTeachFile(filePath);

    return 0;
}

double RobotAPI::GetCurrentAngle(int axisNum)
{
    return  RobotThetaMatrix::Instance()->GetTheta(axisNum);
}

void RobotAPI::GoHomeForAllAxis()
{
    UnifiedRobot::Instance()->SendGoHomeMsg();
}

void RobotAPI::SetForceControl(bool flag)
{
    pUri->EnableForceControl_MedicalUltrasoundRobot(flag);
}

void RobotAPI::StopMoving()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
}

int RobotAPI::GetStatus()
{
    return StatusString::Instance()->GetStatusIndex();
}

int RobotAPI::EnterFastControl(double time)
{
    MSectionInfo msi;
    msi.duration_s = time;
    msi.period_us  = 700.0;
    msi.workMode      = MSectionInfo::StopWaitMode;
    msi.pulseTime     = 500*1000;
    msi.internalCycle = 1;
    msi.totalPoints   = 120;
    msi.dividedPoints = 60;
    msi.enableSendPosition    = MSectionInfo::DisableSend;
    msi.enableCheckConnection = MSectionInfo::DisableCheck;

    const std::string filePath = Configuration::Instance()->GetFilePath(Configuration::RobotMonitoringFilePath);
    if( Teach::ConstructMonitoringTeachFile(msi, filePath) != 0){
        return -1;
    }
    bool startOk = UnifiedRobot::Instance()->StartActionByTeachFile(filePath);
    if(startOk){
        return 0;
    }else{
        return -1;
    }
}

int RobotAPI::SetAngleInFastControl(int axisNum, double absAngle)
{
    if(axisNum<0 || axisNum>=RobotParams::axisNum){
        return -1;
    }

    //MedicalUltrasoundRobot 绝对位置控制
    std::vector<int> actionMethod(RobotParams::axisNum, 0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionMethod[i] = MsgStructure::MonitorCommand::AbsControl;
    }

    std::vector<int> actionAxes(RobotParams::axisNum, 0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionAxes[i] = i;
    }

    //所有轴设为当前位置 目标axisNum设为目标角度
    std::vector<double> actionTheta(RobotParams::axisNum, 0.0);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionTheta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
    }
    actionTheta[axisNum] = absAngle;

    return UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
}
