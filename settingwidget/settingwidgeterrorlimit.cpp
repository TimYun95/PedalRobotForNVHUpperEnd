#include "settingwidgeterrorlimit.h"
#include "ui_settingwidgeterrorlimit.h"

#include <QMessageBox>

SettingWidgetErrorLimit::SettingWidgetErrorLimit(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetErrorLimit)
{
    ui->setupUi(this);

    lineEdit_errorLimitTrans[0] = ui->lineEdit_errorLimitTrans1;
    lineEdit_errorLimitTrans[1] = ui->lineEdit_errorLimitTrans2;
    lineEdit_errorLimitTrans[2] = ui->lineEdit_errorLimitTrans3;
    lineEdit_errorLimitTrans[3] = ui->lineEdit_errorLimitTrans4;
    lineEdit_errorLimitTrans[4] = ui->lineEdit_errorLimitTrans5;
    lineEdit_errorLimitTrans[5] = ui->lineEdit_errorLimitTrans6;

    lineEdit_errorLimitWork[0] = ui->lineEdit_errorLimitWork1;
    lineEdit_errorLimitWork[1] = ui->lineEdit_errorLimitWork2;
    lineEdit_errorLimitWork[2] = ui->lineEdit_errorLimitWork3;
    lineEdit_errorLimitWork[3] = ui->lineEdit_errorLimitWork4;
    lineEdit_errorLimitWork[4] = ui->lineEdit_errorLimitWork5;
    lineEdit_errorLimitWork[5] = ui->lineEdit_errorLimitWork6;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_errorLimitTrans[i]->hide();
        lineEdit_errorLimitWork[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_errorLimitTrans2->hide();
        //no break!
    case 2:
        ui->label_errorLimitTrans3->hide();
        //no break!
    case 3:
        ui->label_errorLimitTrans4->hide();
        //no break!
    case 4:
        ui->label_errorLimitTrans5->hide();
        //no break!
    case 5:
        ui->label_errorLimitTrans6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }

    switch (RobotParams::dof){
    case 1:
        ui->label_errorLimitWork2->hide();
        //no break!
    case 2:
        ui->label_errorLimitWork3->hide();
        //no break!
    case 3:
        ui->label_errorLimitWork4->hide();
        //no break!
    case 4:
        ui->label_errorLimitWork5->hide();
        //no break!
    case 5:
        ui->label_errorLimitWork6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetErrorLimit::~SettingWidgetErrorLimit()
{
    delete ui;
}

void SettingWidgetErrorLimit::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_errorLimitTrans[i], conf.errorLimitTrans[i]);
    }
    for(int i=0;i<RobotParams::dof;i++){
        SetLineEditValue(lineEdit_errorLimitWork[i], conf.errorLimitWork[i]);
    }
}

bool SettingWidgetErrorLimit::StoreParameters(Configuration &conf)
{
    if( !CheckValidity() ){
        QMessageBox::information( NULL, "Scara",QObject::tr("误差限页面设定参数无效!\n该页面参数将不被保存!"));
        LoadParameters(conf);
        return false;
    }

    for(int i=0;i<RobotParams::axisNum;i++){
        conf.errorLimitTrans[i] = GetLineEditValue(lineEdit_errorLimitTrans[i]);
    }
    for(int i=0;i<RobotParams::dof;i++){
        conf.errorLimitWork[i]  = GetLineEditValue(lineEdit_errorLimitWork[i]);
    }
    return true;
}

bool SettingWidgetErrorLimit::CheckValidity()
{
    for(int i=0;i<RobotParams::axisNum;++i){
        int n = GetLineEditValue(lineEdit_errorLimitTrans[i]);
        if(n <= 0){
            QMessageBox::information( NULL, "Scara",QObject::tr("过渡段的误差限需为正数!"));
            return false;
        }
    }
    for(int i=0;i<RobotParams::axisNum;++i){
        int n = GetLineEditValue(lineEdit_errorLimitWork[i]);
        if(n <= 0){
            QMessageBox::information( NULL, "Scara",QObject::tr("工作段的误差限需为正数!"));
            return false;
        }
    }
    return true;
}
