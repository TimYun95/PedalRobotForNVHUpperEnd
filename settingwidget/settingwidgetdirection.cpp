#include "settingwidgetdirection.h"
#include "ui_settingwidgetdirection.h"

SettingWidgetDirection::SettingWidgetDirection(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetDirection)
{
    ui->setupUi(this);

    horizontalSlider_motor[5]=ui->horizontalSlider_motor6;
    horizontalSlider_motor[4]=ui->horizontalSlider_motor5;
    horizontalSlider_motor[3]=ui->horizontalSlider_motor4;
    horizontalSlider_motor[2]=ui->horizontalSlider_motor3;
    horizontalSlider_motor[1]=ui->horizontalSlider_motor2;
    horizontalSlider_motor[0]=ui->horizontalSlider_motor1;

    horizontalSlider_encoder[5]=ui->horizontalSlider_encoder6;
    horizontalSlider_encoder[4]=ui->horizontalSlider_encoder5;
    horizontalSlider_encoder[3]=ui->horizontalSlider_encoder4;
    horizontalSlider_encoder[2]=ui->horizontalSlider_encoder3;
    horizontalSlider_encoder[1]=ui->horizontalSlider_encoder2;
    horizontalSlider_encoder[0]=ui->horizontalSlider_encoder1;

    horizontalSlider_goHome[5]=ui->horizontalSlider_goHome6;
    horizontalSlider_goHome[4]=ui->horizontalSlider_goHome5;
    horizontalSlider_goHome[3]=ui->horizontalSlider_goHome4;
    horizontalSlider_goHome[2]=ui->horizontalSlider_goHome3;
    horizontalSlider_goHome[1]=ui->horizontalSlider_goHome2;
    horizontalSlider_goHome[0]=ui->horizontalSlider_goHome1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        horizontalSlider_motor[i]->hide();
        horizontalSlider_encoder[i]->hide();
        horizontalSlider_goHome[i]->hide();
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

    on_comboBox_currentIndexChanged(0);
}

SettingWidgetDirection::~SettingWidgetDirection()
{
    delete ui;
}

void SettingWidgetDirection::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        horizontalSlider_motor[i]->setValue(conf.motorDirection[i]);
        horizontalSlider_encoder[i]->setValue(conf.encoderDirection[i]);
        horizontalSlider_goHome[i]->setValue(conf.goHomeDirection[i]);
    }
}

bool SettingWidgetDirection::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.motorDirection[i]=horizontalSlider_motor[i]->value();
        conf.encoderDirection[i]=horizontalSlider_encoder[i]->value();
        conf.goHomeDirection[i]=horizontalSlider_goHome[i]->value();
    }
    return true;
}

void SettingWidgetDirection::on_comboBox_currentIndexChanged(int index)
{
    /*先hide 再show，防止显示闪烁*/
    if(index==0){//motor
        ShowSliders_Encoder(false);
        ShowSliders_GoHome(false);
        ShowSliders_Motor(true);
    }else if(index==1){//encoder
        ShowSliders_Motor(false);
        ShowSliders_GoHome(false);
        ShowSliders_Encoder(true);
    }else if(index==2){//goHome
        ShowSliders_Motor(false);
        ShowSliders_Encoder(false);
        ShowSliders_GoHome(true);
    }else{
        PRINTF(LOG_INFO, "%s: the index(%d) has not been defined yet.\n", __func__, index);
    }
}

void SettingWidgetDirection::ShowSliders_Motor(bool flag)
{
    if(flag){
        for(int i=0; i<RobotParams::axisNum; ++i){
            horizontalSlider_motor[i]->show();
        }
    }else{
        for(int i=0; i<RobotParams::axisNum; ++i){
            horizontalSlider_motor[i]->hide();
        }
    }
}

void SettingWidgetDirection::ShowSliders_Encoder(bool flag)
{
    if(flag){
        for(int i=0; i<RobotParams::axisNum; ++i){
            horizontalSlider_encoder[i]->show();
        }
    }else{
        for(int i=0; i<RobotParams::axisNum; ++i){
            horizontalSlider_encoder[i]->hide();
        }
    }
}

void SettingWidgetDirection::ShowSliders_GoHome(bool flag)
{
    if(flag){
        for(int i=0; i<RobotParams::axisNum; ++i){
            horizontalSlider_goHome[i]->show();
        }
    }else{
        for(int i=0; i<RobotParams::axisNum; ++i){
            horizontalSlider_goHome[i]->hide();
        }
    }
}
