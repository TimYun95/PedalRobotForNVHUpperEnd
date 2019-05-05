#include "settingwidgetsingleaxis.h"
#include "ui_settingwidgetsingleaxis.h"

SettingWidgetSingleAxis::SettingWidgetSingleAxis(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetSingleAxis)
{
    ui->setupUi(this);

    lineEdit_btnRatio[0] = ui->lineEdit_btnRatio1; lineEdit_sliderRatio[0] = ui->lineEdit_sliderRatio1;
    lineEdit_btnRatio[1] = ui->lineEdit_btnRatio2; lineEdit_sliderRatio[1] = ui->lineEdit_sliderRatio2;
    lineEdit_btnRatio[2] = ui->lineEdit_btnRatio3; lineEdit_sliderRatio[2] = ui->lineEdit_sliderRatio3;
    lineEdit_btnRatio[3] = ui->lineEdit_btnRatio4; lineEdit_sliderRatio[3] = ui->lineEdit_sliderRatio4;
    lineEdit_btnRatio[4] = ui->lineEdit_btnRatio5; lineEdit_sliderRatio[4] = ui->lineEdit_sliderRatio5;
    lineEdit_btnRatio[5] = ui->lineEdit_btnRatio6; lineEdit_sliderRatio[5] = ui->lineEdit_sliderRatio6;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_btnRatio[i]->hide();
        lineEdit_sliderRatio[i]->hide();
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

SettingWidgetSingleAxis::~SettingWidgetSingleAxis()
{
    delete ui;
}

void SettingWidgetSingleAxis::LoadParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        SetLineEditValue(lineEdit_btnRatio[i], conf.singleAxisBtnRatio[i]);
        SetLineEditValue(lineEdit_sliderRatio[i], conf.singleAxisSliderRatio[i]);
    }
}

bool SettingWidgetSingleAxis::StoreParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        conf.singleAxisBtnRatio[i] = GetLineEditValue(lineEdit_btnRatio[i]);
        conf.singleAxisSliderRatio[i] = GetLineEditValue(lineEdit_sliderRatio[i]);
    }
    return true;
}
