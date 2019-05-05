#include "settingwidgetmanualcontrol.h"
#include "ui_settingwidgetmanualcontrol.h"

SettingWidgetManualControl::SettingWidgetManualControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetManualControl)
{
    ui->setupUi(this);

    lineEdit_speedRatio[0] = ui->lineEdit_speedRatio1;
    lineEdit_speedRatio[1] = ui->lineEdit_speedRatio2;
    lineEdit_speedRatio[2] = ui->lineEdit_speedRatio3;
    lineEdit_speedRatio[3] = ui->lineEdit_speedRatio4;
    lineEdit_speedRatio[4] = ui->lineEdit_speedRatio5;
    lineEdit_speedRatio[5] = ui->lineEdit_speedRatio6;

    for(int i=RobotParams::dof; i<RobotParams::UIdof; ++i){
        lineEdit_speedRatio[i]->hide();
    }

    switch (RobotParams::dof){
    case 1:
        ui->label_dof2->hide();
        //no break!
    case 2:
        ui->label_dof3->hide();
        //no break!
    case 3:
        ui->label_dof4->hide();
        //no break!
    case 4:
        ui->label_dof5->hide();
        //no break!
    case 5:
        ui->label_dof6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetManualControl::~SettingWidgetManualControl()
{
    delete ui;
}

void SettingWidgetManualControl::LoadParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::dof; ++i){
        SetLineEditValue(lineEdit_speedRatio[i], conf.manualSpeedRatio[i]);
    }
}

bool SettingWidgetManualControl::StoreParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::dof; ++i){
        conf.manualSpeedRatio[i] = GetLineEditValue(lineEdit_speedRatio[i]);
    }
    return true;
}
