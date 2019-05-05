#include "settingwidgetpid.h"
#include "ui_settingwidgetpid.h"

SettingWidgetPID::SettingWidgetPID(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPID)
{
    ui->setupUi(this);

    lineEdit_kp[5]=ui->lineEdit_kp6; lineEdit_ki[5]=ui->lineEdit_ki6;
    lineEdit_kp[4]=ui->lineEdit_kp5; lineEdit_ki[4]=ui->lineEdit_ki5;
    lineEdit_kp[3]=ui->lineEdit_kp4; lineEdit_ki[3]=ui->lineEdit_ki4;
    lineEdit_kp[2]=ui->lineEdit_kp3; lineEdit_ki[2]=ui->lineEdit_ki3;
    lineEdit_kp[1]=ui->lineEdit_kp2; lineEdit_ki[1]=ui->lineEdit_ki2;
    lineEdit_kp[0]=ui->lineEdit_kp1; lineEdit_ki[0]=ui->lineEdit_ki1;

    lineEdit_kd[5]=ui->lineEdit_kd6; lineEdit_kf[5]=ui->lineEdit_kf6;
    lineEdit_kd[4]=ui->lineEdit_kd5; lineEdit_kf[4]=ui->lineEdit_kf5;
    lineEdit_kd[3]=ui->lineEdit_kd4; lineEdit_kf[3]=ui->lineEdit_kf4;
    lineEdit_kd[2]=ui->lineEdit_kd3; lineEdit_kf[2]=ui->lineEdit_kf3;
    lineEdit_kd[1]=ui->lineEdit_kd2; lineEdit_kf[1]=ui->lineEdit_kf2;
    lineEdit_kd[0]=ui->lineEdit_kd1; lineEdit_kf[0]=ui->lineEdit_kf1;

    lineEdit_kp_work[5]=ui->lineEdit_kp_work6; lineEdit_ki_work[5]=ui->lineEdit_ki_work6;
    lineEdit_kp_work[4]=ui->lineEdit_kp_work5; lineEdit_ki_work[4]=ui->lineEdit_ki_work5;
    lineEdit_kp_work[3]=ui->lineEdit_kp_work4; lineEdit_ki_work[3]=ui->lineEdit_ki_work4;
    lineEdit_kp_work[2]=ui->lineEdit_kp_work3; lineEdit_ki_work[2]=ui->lineEdit_ki_work3;
    lineEdit_kp_work[1]=ui->lineEdit_kp_work2; lineEdit_ki_work[1]=ui->lineEdit_ki_work2;
    lineEdit_kp_work[0]=ui->lineEdit_kp_work1; lineEdit_ki_work[0]=ui->lineEdit_ki_work1;

    lineEdit_kd_work[5]=ui->lineEdit_kd_work6; lineEdit_kf_work[5]=ui->lineEdit_kf_work6;
    lineEdit_kd_work[4]=ui->lineEdit_kd_work5; lineEdit_kf_work[4]=ui->lineEdit_kf_work5;
    lineEdit_kd_work[3]=ui->lineEdit_kd_work4; lineEdit_kf_work[3]=ui->lineEdit_kf_work4;
    lineEdit_kd_work[2]=ui->lineEdit_kd_work3; lineEdit_kf_work[2]=ui->lineEdit_kf_work3;
    lineEdit_kd_work[1]=ui->lineEdit_kd_work2; lineEdit_kf_work[1]=ui->lineEdit_kf_work2;
    lineEdit_kd_work[0]=ui->lineEdit_kd_work1; lineEdit_kf_work[0]=ui->lineEdit_kf_work1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_kp[i]->hide();
        lineEdit_ki[i]->hide();
        lineEdit_kd[i]->hide();
        lineEdit_kf[i]->hide();
        lineEdit_kp_work[i]->hide();
        lineEdit_ki_work[i]->hide();
        lineEdit_kd_work[i]->hide();
        lineEdit_kf_work[i]->hide();
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

    on_comboBox_currentIndexChanged(0);
}

SettingWidgetPID::~SettingWidgetPID()
{
    delete ui;
}

void SettingWidgetPID::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_kp[i], conf.kp[i]);
        SetLineEditValue(lineEdit_ki[i], conf.ki[i]);
        SetLineEditValue(lineEdit_kd[i], conf.kd[i]);
        SetLineEditValue(lineEdit_kf[i], conf.kf[i]);
        SetLineEditValue(lineEdit_kp_work[i], conf.kp_work[i]);
        SetLineEditValue(lineEdit_ki_work[i], conf.ki_work[i]);
        SetLineEditValue(lineEdit_kd_work[i], conf.kd_work[i]);
        SetLineEditValue(lineEdit_kf_work[i], conf.kf_work[i]);
    }
}

bool SettingWidgetPID::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.kp[i]=GetLineEditValue(lineEdit_kp[i]);
        conf.ki[i]=GetLineEditValue(lineEdit_ki[i]);
        conf.kd[i]=GetLineEditValue(lineEdit_kd[i]);
        conf.kf[i]=GetLineEditValue(lineEdit_kf[i]);
        conf.kp_work[i]=GetLineEditValue(lineEdit_kp_work[i]);
        conf.ki_work[i]=GetLineEditValue(lineEdit_ki_work[i]);
        conf.kd_work[i]=GetLineEditValue(lineEdit_kd_work[i]);
        conf.kf_work[i]=GetLineEditValue(lineEdit_kf_work[i]);
    }
    return true;
}

void SettingWidgetPID::on_comboBox_currentIndexChanged(int index)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        if(index==0){ /* trans */
            lineEdit_kp[i]->show(); lineEdit_ki[i]->show(); lineEdit_kd[i]->show(); lineEdit_kf[i]->show();
            lineEdit_kp_work[i]->hide(); lineEdit_ki_work[i]->hide(); lineEdit_kd_work[i]->hide(); lineEdit_kf_work[i]->hide();
        }else if(index==1){ /* work */
            lineEdit_kp[i]->hide(); lineEdit_ki[i]->hide(); lineEdit_kd[i]->hide(); lineEdit_kf[i]->hide();
            lineEdit_kp_work[i]->show(); lineEdit_ki_work[i]->show(); lineEdit_kd_work[i]->show(); lineEdit_kf_work[i]->show();
        }
    }
}
