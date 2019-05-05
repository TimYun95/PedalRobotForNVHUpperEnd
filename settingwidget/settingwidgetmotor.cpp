#include "settingwidgetmotor.h"
#include "ui_settingwidgetmotor.h"

SettingWidgetMotor::SettingWidgetMotor(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetMotor)
{
    ui->setupUi(this);

    lineEdit_motorRatio[5]=ui->lineEdit_motorRatio6; lineEdit_motorSubdivision[5]=ui->lineEdit_motorSubdivision6;
    lineEdit_motorRatio[4]=ui->lineEdit_motorRatio5; lineEdit_motorSubdivision[4]=ui->lineEdit_motorSubdivision5;
    lineEdit_motorRatio[3]=ui->lineEdit_motorRatio4; lineEdit_motorSubdivision[3]=ui->lineEdit_motorSubdivision4;
    lineEdit_motorRatio[2]=ui->lineEdit_motorRatio3; lineEdit_motorSubdivision[2]=ui->lineEdit_motorSubdivision3;
    lineEdit_motorRatio[1]=ui->lineEdit_motorRatio2; lineEdit_motorSubdivision[1]=ui->lineEdit_motorSubdivision2;
    lineEdit_motorRatio[0]=ui->lineEdit_motorRatio1; lineEdit_motorSubdivision[0]=ui->lineEdit_motorSubdivision1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_motorRatio[i]->hide();
        lineEdit_motorSubdivision[i]->hide();
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

SettingWidgetMotor::~SettingWidgetMotor()
{
    delete ui;
}

void SettingWidgetMotor::LoadParameters(Configuration &conf)
{

    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_motorRatio[i], conf.motorRatio[i]);
        SetLineEditValue(lineEdit_motorSubdivision[i], conf.motorSubdivision[i]);
    }
}

bool SettingWidgetMotor::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.motorRatio[i]=GetLineEditValue(lineEdit_motorRatio[i]);
        conf.motorSubdivision[i]=GetLineEditValue(lineEdit_motorSubdivision[i]);
    }
    return true;
}
