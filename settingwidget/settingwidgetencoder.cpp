#include "settingwidgetencoder.h"
#include "ui_settingwidgetencoder.h"

SettingWidgetEncoder::SettingWidgetEncoder(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetEncoder)
{
    ui->setupUi(this);

    lineEdit_encoderRatio[5]=ui->lineEdit_encoderRatio6; lineEdit_encoderSubdivision[5]=ui->lineEdit_encoderSubdivision6;
    lineEdit_encoderRatio[4]=ui->lineEdit_encoderRatio5; lineEdit_encoderSubdivision[4]=ui->lineEdit_encoderSubdivision5;
    lineEdit_encoderRatio[3]=ui->lineEdit_encoderRatio4; lineEdit_encoderSubdivision[3]=ui->lineEdit_encoderSubdivision4;
    lineEdit_encoderRatio[2]=ui->lineEdit_encoderRatio3; lineEdit_encoderSubdivision[2]=ui->lineEdit_encoderSubdivision3;
    lineEdit_encoderRatio[1]=ui->lineEdit_encoderRatio2; lineEdit_encoderSubdivision[1]=ui->lineEdit_encoderSubdivision2;
    lineEdit_encoderRatio[0]=ui->lineEdit_encoderRatio1; lineEdit_encoderSubdivision[0]=ui->lineEdit_encoderSubdivision1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_encoderRatio[i]->hide();
        lineEdit_encoderSubdivision[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_2->hide();
        ui->label_12->hide();
        //no break!
    case 2:
        ui->label_3->hide();
        ui->label_13->hide();
        //no break!
    case 3:
        ui->label_4->hide();
        ui->label_14->hide();
        //no break!
    case 4:
        ui->label_5->hide();
        ui->label_15->hide();
        //no break!
    case 5:
        ui->label_6->hide();
        ui->label_16->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetEncoder::~SettingWidgetEncoder()
{
    delete ui;
}

void SettingWidgetEncoder::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_encoderRatio[i], conf.encoderRatio[i]);
        SetLineEditValue(lineEdit_encoderSubdivision[i], conf.encoderSubdivision[i]);
    }
}

bool SettingWidgetEncoder::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.encoderRatio[i]=GetLineEditValue(lineEdit_encoderRatio[i]);
        conf.encoderSubdivision[i]=GetLineEditValue(lineEdit_encoderSubdivision[i]);
    }
    return true;
}
