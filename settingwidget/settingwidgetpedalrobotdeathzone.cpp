#include "settingwidgetpedalrobotdeathzone.h"
#include "ui_settingwidgetpedalrobotdeathzone.h"

#include <QMessageBox>

SettingWidgetPedalRobotDeathZone::SettingWidgetPedalRobotDeathZone(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotDeathZone)
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;
}

SettingWidgetPedalRobotDeathZone::~SettingWidgetPedalRobotDeathZone()
{
    delete ui;
}

void SettingWidgetPedalRobotDeathZone::LoadParameters(Configuration &conf)
{
    //监听模式的正极限
    SetLineEditValue(ui->lineEdit_brakePositionLimit, conf.pedalPositionLimit[0]);
    SetLineEditValue(ui->lineEdit_accPositionLimit, conf.pedalPositionLimit[1]);

    //死区开度
    SetLineEditValue(ui->lineEdit_brakeDeathOpenValue, conf.deathOpenValue[0]);
    SetLineEditValue(ui->lineEdit_accDeathOpenValue, conf.deathOpenValue[1]);

    SetLineEditValue(ui->lineEdit_brakeThetaAfterGoHome, conf.brakeThetaAfterGoHome);
}

bool SettingWidgetPedalRobotDeathZone::StoreParameters(Configuration &conf)
{
    double brakeLimit = GetLineEditValue(ui->lineEdit_brakePositionLimit);
    double accLimit = GetLineEditValue(ui->lineEdit_accPositionLimit);
    //设定的监听模式的正极限 必须在 软件限位的正极限内
    if(brakeLimit > conf.limitPos[0]){
        QMessageBox::warning(NULL, "Warning", QObject::tr("死区:\n刹车踏板设定的极限位置需小于软件限位!")+QString::number(brakeLimit)+"/"+QString::number(conf.limitPos[0]));
        return false;
    }else if(accLimit > conf.limitPos[1]){
        QMessageBox::warning(NULL, "Warning", QObject::tr("死区:\n油门踏板设定的极限位置需小于软件限位!")+QString::number(accLimit)+"/"+QString::number(conf.limitPos[1]));
        return false;
    }

    double brakeThetaAfterGoHome = GetLineEditValue(ui->lineEdit_brakeThetaAfterGoHome);
    if(brakeThetaAfterGoHome > conf.limitPos[0]){
        QMessageBox::warning(NULL, "Warning", QObject::tr("死区:\n回原后的刹车位置需小于软件限位!")+QString::number(brakeLimit)+"/"+QString::number(conf.limitPos[0]));
        return false;
    }

    /* 保存 */
    //监听模式的正极限
    conf.pedalPositionLimit[0] = brakeLimit;
    conf.pedalPositionLimit[1] = accLimit;
    //死区开度
    conf.deathOpenValue[0] = GetLineEditValue(ui->lineEdit_brakeDeathOpenValue);
    conf.deathOpenValue[1] = GetLineEditValue(ui->lineEdit_accDeathOpenValue);

    conf.brakeThetaAfterGoHome = brakeThetaAfterGoHome;
    return true;
}
