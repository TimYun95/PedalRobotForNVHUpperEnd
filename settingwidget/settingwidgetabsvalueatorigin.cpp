#include "settingwidgetabsvalueatorigin.h"
#include "ui_settingwidgetabsvalueatorigin.h"
/*注意：该UI只用于伺服电机的机器人*/

SettingWidgetAbsValueAtOrigin::SettingWidgetAbsValueAtOrigin(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetAbsValueAtOrigin)
{
    ui->setupUi(this);

    lineEdit_singleAbsValue[5]=ui->lineEdit_singleAbsValue_6; lineEdit_multiAbsValue[5]=ui->lineEdit_multiAbsValue_6;
    lineEdit_singleAbsValue[4]=ui->lineEdit_singleAbsValue_5; lineEdit_multiAbsValue[4]=ui->lineEdit_multiAbsValue_5;
    lineEdit_singleAbsValue[3]=ui->lineEdit_singleAbsValue_4; lineEdit_multiAbsValue[3]=ui->lineEdit_multiAbsValue_4;
    lineEdit_singleAbsValue[2]=ui->lineEdit_singleAbsValue_3; lineEdit_multiAbsValue[2]=ui->lineEdit_multiAbsValue_3;
    lineEdit_singleAbsValue[1]=ui->lineEdit_singleAbsValue_2; lineEdit_multiAbsValue[1]=ui->lineEdit_multiAbsValue_2;
    lineEdit_singleAbsValue[0]=ui->lineEdit_singleAbsValue_1; lineEdit_multiAbsValue[0]=ui->lineEdit_multiAbsValue_1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_singleAbsValue[i]->hide();
        lineEdit_multiAbsValue[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_single2->hide();
        ui->label_multi2->hide();
        //no break!
    case 2:
        ui->label_single3->hide();
        ui->label_multi3->hide();
        //no break!
    case 3:
        ui->label_single4->hide();
        ui->label_multi4->hide();
        //no break!
    case 4:
        ui->label_single5->hide();
        ui->label_multi5->hide();
        //no break!
    case 5:
        ui->label_single6->hide();
        ui->label_multi6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetAbsValueAtOrigin::~SettingWidgetAbsValueAtOrigin()
{
    delete ui;
}

void SettingWidgetAbsValueAtOrigin::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_singleAbsValue[i], conf.singleAbsValue[i]);
        SetLineEditValue(lineEdit_multiAbsValue[i], conf.multiAbsValue[i]);
    }
}

bool SettingWidgetAbsValueAtOrigin::StoreParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        conf.singleAbsValue[i]= GetLineEditValue(lineEdit_singleAbsValue[i]);
        conf.multiAbsValue[i] = GetLineEditValue(lineEdit_multiAbsValue[i]);
    }
    return true;
}
