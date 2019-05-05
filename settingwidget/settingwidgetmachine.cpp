#include "settingwidgetmachine.h"
#include "ui_settingwidgetmachine.h"

SettingWidgetMachine::SettingWidgetMachine(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetMachine)
{
    ui->setupUi(this);

    lineEdit_homeAngle[5]=ui->lineEdit_homeAngle_6;
    lineEdit_homeAngle[4]=ui->lineEdit_homeAngle_5;
    lineEdit_homeAngle[3]=ui->lineEdit_homeAngle_4;
    lineEdit_homeAngle[2]=ui->lineEdit_homeAngle_3;
    lineEdit_homeAngle[1]=ui->lineEdit_homeAngle_2;
    lineEdit_homeAngle[0]=ui->lineEdit_homeAngle_1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_homeAngle[i]->hide();
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

SettingWidgetMachine::~SettingWidgetMachine()
{
    delete ui;
}

void SettingWidgetMachine::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_homeAngle[i], conf.homeAngle[i]);
    }
}

bool SettingWidgetMachine::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.homeAngle[i]=GetLineEditValue(lineEdit_homeAngle[i]);
    }
    return true;
}
