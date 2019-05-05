#include "settingwidgetspeed.h"
#include "ui_settingwidgetspeed.h"

SettingWidgetSpeed::SettingWidgetSpeed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetSpeed)
{
    ui->setupUi(this);

    lineEdit_speedLimit[5]=ui->lineEdit_speedLimit6; lineEdit_pulseFilter[5]=ui->lineEdit_pulseFilter6; lineEdit_motorAcceleration[5]=ui->lineEdit_motorAcceleration6;
    lineEdit_speedLimit[4]=ui->lineEdit_speedLimit5; lineEdit_pulseFilter[4]=ui->lineEdit_pulseFilter5; lineEdit_motorAcceleration[4]=ui->lineEdit_motorAcceleration5;
    lineEdit_speedLimit[3]=ui->lineEdit_speedLimit4; lineEdit_pulseFilter[3]=ui->lineEdit_pulseFilter4; lineEdit_motorAcceleration[3]=ui->lineEdit_motorAcceleration4;
    lineEdit_speedLimit[2]=ui->lineEdit_speedLimit3; lineEdit_pulseFilter[2]=ui->lineEdit_pulseFilter3; lineEdit_motorAcceleration[2]=ui->lineEdit_motorAcceleration3;
    lineEdit_speedLimit[1]=ui->lineEdit_speedLimit2; lineEdit_pulseFilter[1]=ui->lineEdit_pulseFilter2; lineEdit_motorAcceleration[1]=ui->lineEdit_motorAcceleration2;
    lineEdit_speedLimit[0]=ui->lineEdit_speedLimit1; lineEdit_pulseFilter[0]=ui->lineEdit_pulseFilter1; lineEdit_motorAcceleration[0]=ui->lineEdit_motorAcceleration1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_speedLimit[i]->hide();
        lineEdit_pulseFilter[i]->hide();
        lineEdit_motorAcceleration[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_axis2->hide();
        //no break!
    case 2:
        ui->label_axis3->hide();
        //no break!
    case 3:
        ui->label_axis4->hide();
        //no break!
    case 4:
        ui->label_axis5->hide();
        //no break!
    case 5:
        ui->label_axis6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetSpeed::~SettingWidgetSpeed()
{
    delete ui;
}

void SettingWidgetSpeed::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_speedLimit[i], conf.speedLimit[i]);
        SetLineEditValue(lineEdit_pulseFilter[i], conf.pulseFilter[i]);
        SetLineEditValue(lineEdit_motorAcceleration[i], conf.motorAcceleration[i]);
    }
}

bool SettingWidgetSpeed::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.speedLimit[i]=GetLineEditValue(lineEdit_speedLimit[i]);
        conf.pulseFilter[i]=GetLineEditValue(lineEdit_pulseFilter[i]);
        conf.motorAcceleration[i]=GetLineEditValue(lineEdit_motorAcceleration[i]);
    }
    return true;
}
