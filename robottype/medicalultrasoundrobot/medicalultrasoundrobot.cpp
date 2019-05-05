#include "medicalultrasoundrobot.h"

#include <math.h>

#include <QString>
#include <QFileDialog>
#include <QMessageBox>

#include "common/printf.h"
#include "communication/libmodbus/EndianConverter.h"
#include "communication/msgtypes.h"

#include "communicationmediator.h"

#include "common/message.h"
#include "common/globalvariables.h"

MedicalUltrasoundRobot::MedicalUltrasoundRobot(QObject *parent)
    : QObject(parent), conf(Configuration::Instance())
{
    adc=new adcReader();
    forceValue=0.0;
    isForceControlMode=false;

    largeForceCount=0;
    AlarmCount=0;

    isGoingHome=false;
}

MedicalUltrasoundRobot::~MedicalUltrasoundRobot()
{
    delete adc;
}

void MedicalUltrasoundRobot::EnableForceControl(bool checked)
{
    //TimerDone10ms()的SendForceValue() 在isForceControlMode==true时 不断发送指令数据
    isForceControlMode=checked;

    if(!isForceControlMode){
        StopRobot();
    }
}

void MedicalUltrasoundRobot::CheckADForce()
{
    adc->ReadData();

    static int count=0;
    if(++count%20 != 0){
        return;
    }
    QString forceStatus;
    if (!adc->isADValid()) {
        forceStatus = QObject::tr("AD转换未启动!");
    }else{
        double force = adc->GetForceValue();
        forceValue  = -force + conf->balanceForce;//校正
        forceStatus = QString::number(forceValue,'f',1) + QObject::tr("kg");
    }
    emit ForceValueSignal(forceStatus);

    SendForceValue();
    LargeForceAlarm();
}

int MedicalUltrasoundRobot::CheckADButton()
{
    int button=-1;//unvalid button
    if(adc->CheckButton()) {
        button = adc->pressedButtons;
    }

    if(adc->isForceButtonClicked()) {//力控开关
        EnableForceControl(!isForceControlMode);
    }
    return button;
}

void MedicalUltrasoundRobot::SendForceValue()
{
    if(isForceControlMode) {
        MsgStructure::MedicalForceValue mfv(forceValue);
        CommunicationMediator::Instance()->SendMsg(mfv, CommunicationMediator::DownlinkMsg);
    }
}

void MedicalUltrasoundRobot::LargeForceAlarm()//力过大告警
{
    static bool forceAlarm=false;
    if(forceAlarm){//防止重复提醒
        return;
    }

    //力过大计数 防止误判
    int statusIndex = StatusString::Instance()->GetStatusIndex();
    if (fabs(forceValue) > fabs(conf->forceValueThres)//力过大
         && (statusIndex != StatusString::Status_ConnectiongCommunication)//不是 刚开机
         && (statusIndex != StatusString::Status_RobotIsClosed)//不是 关机
         && (statusIndex != StatusString::Status_LostCommunication))//不是 丢失连接
    {
        if(++largeForceCount < 5){//多次检测
            return;
        }
        largeForceCount=10;

        StopRobot();
        forceAlarm=true;
        {QMessageBox::information(NULL, "Warning", QObject::tr("力传感器检测到物体!\n机器人停止运动"));}
        forceAlarm=false;
        largeForceCount=0;
    } else {
        forceAlarm=false;
        largeForceCount=0;
    }
}

void MedicalUltrasoundRobot::StopRobot()
{
    CommunicationMediator::Instance()->SendMsg(I2C_EnterIdleState, CommunicationMediator::DownlinkMsg);
}

void MedicalUltrasoundRobot::ResetAlarmSignal()
{
    AlarmCount=0;
}

void MedicalUltrasoundRobot::CheckServoAlarmSignal(const int inStatus)
{
    //伺服电机的ALARM信号 接在转接板的光电接口
    bool alarm[3];
    alarm[0] = inStatus & (1<<4);
    alarm[1] = inStatus & (1<<5);
    alarm[2] = inStatus & (1<<6);
    int statusIndex = StatusString::Instance()->GetStatusIndex();
    if ( !alarm[0] || !alarm[1] || !alarm[2] ){//alarm=false: ALARM signal
        if (++AlarmCount<300 || statusIndex == StatusString::Status_RobotIsClosed || statusIndex == StatusString::Status_LostCommunication){
            //多次检测 or 已关机 or 丢失连接
        }else{//ALARM
            for (int i=0; i<3; ++i){
                if (alarm[i]==false){
                    StopRobot();//停止
                    QMessageBox::information(NULL, "Warning", QString::number(i+2) +//2~4号轴用伺服电机
                                             QString("#电机ALARM故障!\n机器已停止运行!")) ;
                }
            }
        }
    }

    AlarmCount=0;
}

void MedicalUltrasoundRobot::ForceControlClickedSlot(bool checked)
{
    PRINTF(LOG_DEBUG, "%s...checked=%d\n",__func__, checked);
    EnableForceControl(checked);
}
