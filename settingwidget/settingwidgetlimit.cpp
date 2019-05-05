#include "settingwidgetlimit.h"
#include "ui_settingwidgetlimit.h"

#include "common/globalvariables.h"

SettingWidgetLimit::SettingWidgetLimit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetLimit)
{
    ui->setupUi(this);

    lineEdit_pos[5]=ui->lineEdit_pos6; lineEdit_neg[5]=ui->lineEdit_neg6;
    lineEdit_pos[4]=ui->lineEdit_pos5; lineEdit_neg[4]=ui->lineEdit_neg5;
    lineEdit_pos[3]=ui->lineEdit_pos4; lineEdit_neg[3]=ui->lineEdit_neg4;
    lineEdit_pos[2]=ui->lineEdit_pos3; lineEdit_neg[2]=ui->lineEdit_neg3;
    lineEdit_pos[1]=ui->lineEdit_pos2; lineEdit_neg[1]=ui->lineEdit_neg2;
    lineEdit_pos[0]=ui->lineEdit_pos1; lineEdit_neg[0]=ui->lineEdit_neg1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_pos[i]->hide();
        lineEdit_neg[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_2->hide();
        ui->label_12->hide();
        ui->pushButton_neg2->hide();
        ui->pushButton_pos2->hide();
        //no break!
    case 2:
        ui->label_3->hide();
        ui->label_13->hide();
        ui->pushButton_neg3->hide();
        ui->pushButton_pos3->hide();
        //no break!
    case 3:
        ui->label_4->hide();
        ui->label_14->hide();
        ui->pushButton_neg4->hide();
        ui->pushButton_pos4->hide();
        //no break!
    case 4:
        ui->label_5->hide();
        ui->label_15->hide();
        ui->pushButton_neg5->hide();
        ui->pushButton_pos5->hide();
        //no break!
    case 5:
        ui->label_6->hide();
        ui->label_16->hide();
        ui->pushButton_neg6->hide();
        ui->pushButton_pos6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetLimit::~SettingWidgetLimit()
{
    delete ui;
}

void SettingWidgetLimit::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_pos[i], conf.limitPos[i]);
        SetLineEditValue(lineEdit_neg[i], conf.limitNeg[i]);
    }
}

bool SettingWidgetLimit::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.limitPos[i]=GetLineEditValue(lineEdit_pos[i]);
        conf.limitNeg[i]=GetLineEditValue(lineEdit_neg[i]);
    }
    return true;
}

void SettingWidgetLimit::SetToCurrentPosition()
{
    double robotTheta[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        robotTheta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
    }

    if     (sender()==ui->pushButton_pos1) SetLineEditValue(ui->lineEdit_pos1, robotTheta[0]);
    else if(sender()==ui->pushButton_pos2) SetLineEditValue(ui->lineEdit_pos2, robotTheta[1]);
    else if(sender()==ui->pushButton_pos3) SetLineEditValue(ui->lineEdit_pos3, robotTheta[2]);
    else if(sender()==ui->pushButton_pos4) SetLineEditValue(ui->lineEdit_pos4, robotTheta[3]);
    else if(sender()==ui->pushButton_pos5) SetLineEditValue(ui->lineEdit_pos5, robotTheta[4]);
    else if(sender()==ui->pushButton_pos6) SetLineEditValue(ui->lineEdit_pos6, robotTheta[5]);
    else if(sender()==ui->pushButton_neg1) SetLineEditValue(ui->lineEdit_neg1, robotTheta[0]);
    else if(sender()==ui->pushButton_neg2) SetLineEditValue(ui->lineEdit_neg2, robotTheta[1]);
    else if(sender()==ui->pushButton_neg3) SetLineEditValue(ui->lineEdit_neg3, robotTheta[2]);
    else if(sender()==ui->pushButton_neg4) SetLineEditValue(ui->lineEdit_neg4, robotTheta[3]);
    else if(sender()==ui->pushButton_neg5) SetLineEditValue(ui->lineEdit_neg5, robotTheta[4]);
    else if(sender()==ui->pushButton_neg6) SetLineEditValue(ui->lineEdit_neg6, robotTheta[5]);
}
