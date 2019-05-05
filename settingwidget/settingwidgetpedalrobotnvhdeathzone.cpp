#include "settingwidgetpedalrobotnvhdeathzone.h"
#include "ui_settingwidgetpedalrobotnvhdeathzone.h"

#include <QMessageBox>

SettingWidgetPedalRobotNVHDeathZone::SettingWidgetPedalRobotNVHDeathZone(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotNVHDeathZone)
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;

#ifndef ENABLE_BRAKE_NVH
    ui->groupBox_brk->setVisible(false);
#endif
}

SettingWidgetPedalRobotNVHDeathZone::~SettingWidgetPedalRobotNVHDeathZone()
{
    delete ui;
}

void SettingWidgetPedalRobotNVHDeathZone::LoadParameters(Configuration &conf)
{
    // 负极限
    SetLineEditValue(ui->lineEdit_brkneg, conf.limitNeg[0]);
    SetLineEditValue(ui->lineEdit_accneg, conf.limitNeg[1]);

    // 正极限
    SetLineEditValue(ui->lineEdit_brkpos, conf.limitPos[0]);
    SetLineEditValue(ui->lineEdit_accpos, conf.limitPos[1]);

    // 怠速位置
    SetLineEditValue(ui->lineEdit_ispeed, conf.brakeThetaAfterGoHome);
}

bool SettingWidgetPedalRobotNVHDeathZone::StoreParameters(Configuration &conf)
{
    // 负极限
    conf.limitNeg[0] = GetLineEditValue(ui->lineEdit_brkneg);
    conf.limitNeg[1] = GetLineEditValue(ui->lineEdit_accneg);

    // 正极限
    conf.limitPos[0] = GetLineEditValue(ui->lineEdit_brkpos);
    conf.limitPos[1] = GetLineEditValue(ui->lineEdit_accpos);

    // 怠速位置
    double initialSpeedBrakePosition = GetLineEditValue(ui->lineEdit_ispeed);
    if(initialSpeedBrakePosition > conf.limitPos[0] || initialSpeedBrakePosition < conf.limitNeg[0])
    {
        QMessageBox::warning(NULL, "Warning", QObject::tr("近怠速位置超出极限范围!"));
        return false;
    }

    conf.brakeThetaAfterGoHome = initialSpeedBrakePosition;
    return true;
}
