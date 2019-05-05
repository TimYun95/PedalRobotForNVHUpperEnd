#include "settingwidgetgohomespeed.h"
#include "ui_settingwidgetgohomespeed.h"

SettingWidgetGoHomeSpeed::SettingWidgetGoHomeSpeed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetGoHomeSpeed)
{
    ui->setupUi(this);

    lineEdit_highSpeed[5]=ui->lineEdit_highSpeed6; lineEdit_lowSpeed[5]=ui->lineEdit_lowSpeed6;
    lineEdit_highSpeed[4]=ui->lineEdit_highSpeed5; lineEdit_lowSpeed[4]=ui->lineEdit_lowSpeed5;
    lineEdit_highSpeed[3]=ui->lineEdit_highSpeed4; lineEdit_lowSpeed[3]=ui->lineEdit_lowSpeed4;
    lineEdit_highSpeed[2]=ui->lineEdit_highSpeed3; lineEdit_lowSpeed[2]=ui->lineEdit_lowSpeed3;
    lineEdit_highSpeed[1]=ui->lineEdit_highSpeed2; lineEdit_lowSpeed[1]=ui->lineEdit_lowSpeed2;
    lineEdit_highSpeed[0]=ui->lineEdit_highSpeed1; lineEdit_lowSpeed[0]=ui->lineEdit_lowSpeed1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_highSpeed[i]->hide();
        lineEdit_lowSpeed[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_2->hide();
        //no break!
    case 2:
        ui->label_3->hide();
        //no break!
    case 3:
        ui->label_4->hide();
        //no break!
    case 4:
        ui->label_5->hide();
        //no break!
    case 5:
        ui->label_6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetGoHomeSpeed::~SettingWidgetGoHomeSpeed()
{
    delete ui;
}

void SettingWidgetGoHomeSpeed::LoadParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        SetLineEditValue(lineEdit_highSpeed[i], conf.goHomeHighSpeed[i]);
        SetLineEditValue(lineEdit_lowSpeed[i], conf.goHomeLowSpeed[i]);
    }
}

bool SettingWidgetGoHomeSpeed::StoreParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        conf.goHomeHighSpeed[i] = GetLineEditValue(lineEdit_highSpeed[i]);
        conf.goHomeLowSpeed[i] = GetLineEditValue(lineEdit_lowSpeed[i]);
    }
    return true;
}
