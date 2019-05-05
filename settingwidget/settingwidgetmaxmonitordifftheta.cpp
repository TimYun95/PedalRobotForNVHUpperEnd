#include "settingwidgetmaxmonitordifftheta.h"
#include "ui_settingwidgetmaxmonitordifftheta.h"

SettingWidgetMaxMonitorDiffTheta::SettingWidgetMaxMonitorDiffTheta(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetMaxMonitorDiffTheta)
{
    ui->setupUi(this);

    lineEdit_maxMonitorDiffTheta[0] = ui->lineEdit_maxMonitorDiffTheta1;
    lineEdit_maxMonitorDiffTheta[1] = ui->lineEdit_maxMonitorDiffTheta2;
    lineEdit_maxMonitorDiffTheta[2] = ui->lineEdit_maxMonitorDiffTheta3;
    lineEdit_maxMonitorDiffTheta[3] = ui->lineEdit_maxMonitorDiffTheta4;
    lineEdit_maxMonitorDiffTheta[4] = ui->lineEdit_maxMonitorDiffTheta5;
    lineEdit_maxMonitorDiffTheta[5] = ui->lineEdit_maxMonitorDiffTheta6;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_maxMonitorDiffTheta[i]->hide();
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

SettingWidgetMaxMonitorDiffTheta::~SettingWidgetMaxMonitorDiffTheta()
{
    delete ui;
}

void SettingWidgetMaxMonitorDiffTheta::LoadParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        SetLineEditValue(lineEdit_maxMonitorDiffTheta[i], conf.maxMonitorDiffTheta[i]);
    }
}

bool SettingWidgetMaxMonitorDiffTheta::StoreParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        conf.maxMonitorDiffTheta[i] = GetLineEditValue(lineEdit_maxMonitorDiffTheta[i]);
    }
    return true;
}
