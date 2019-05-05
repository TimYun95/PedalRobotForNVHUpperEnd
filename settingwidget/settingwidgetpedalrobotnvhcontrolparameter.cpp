#include "settingwidgetpedalrobotnvhcontrolparameter.h"
#include "ui_settingwidgetpedalrobotnvhcontrolparameter.h"

SettingWidgetPedalRobotNVHControlParameter::SettingWidgetPedalRobotNVHControlParameter(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotNVHControlParameter)
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;
}

SettingWidgetPedalRobotNVHControlParameter::~SettingWidgetPedalRobotNVHControlParameter()
{
    delete ui;
}

void SettingWidgetPedalRobotNVHControlParameter::LoadParameters(Configuration &conf)
{
    // Loop
    ui->lineEdit_loop_pedalgain->setText(QString::number(conf.sysControlParams[2], 'f', 2));
    ui->lineEdit_loop_threshold->setText(QString::number(conf.sysControlParams[3], 'f', 1));

    // DAS-CT
    ui->lineEdit_dasct_accgain->setText(QString::number(conf.sysControlParams[4], 'f', 2));
    ui->lineEdit_dasct_rectifytime->setText(QString::number(conf.sysControlParams[5], 'f', 1));

    // CS
    ui->lineEdit_cs_accgain->setText(QString::number(conf.sysControlParamsWltc[0], 'f', 2));
    ui->lineEdit_cs_rectifytime->setText(QString::number(conf.sysControlParamsWltc[1], 'f', 1));

    // HLGS
    ui->lineEdit_hlgs_acc->setText(QString::number(conf.sysControlParamsWltc[2], 'f', 1));
    ui->lineEdit_hlgs_approachtime->setText(QString::number(conf.sysControlParamsWltc[3], 'f', 1));
    ui->lineEdit_hlgs_accgain->setText(QString::number(conf.sysControlParamsWltc[4], 'f', 2));
    ui->lineEdit_hlgs_advancedov->setText(QString::number(conf.sysControlParamsWltc[5], 'f', 1));

    // APS
    ui->lineEdit_aps_advancedov->setText(QString::number(conf.sysControlParamsWltc[6], 'f', 1));
}

bool SettingWidgetPedalRobotNVHControlParameter::StoreParameters(Configuration &conf)
{
    // Loop
    conf.sysControlParams[2] = GetLineEditValue(ui->lineEdit_loop_pedalgain);
    conf.sysControlParams[3] = GetLineEditValue(ui->lineEdit_loop_threshold);

    // DAS-CT
    conf.sysControlParams[4] = GetLineEditValue(ui->lineEdit_dasct_accgain);
    conf.sysControlParams[5] = GetLineEditValue(ui->lineEdit_dasct_rectifytime);

    // CS
    conf.sysControlParamsWltc[0] = GetLineEditValue(ui->lineEdit_cs_accgain);
    conf.sysControlParamsWltc[1] = GetLineEditValue(ui->lineEdit_cs_rectifytime);

    // HLGS
    conf.sysControlParamsWltc[2] = GetLineEditValue(ui->lineEdit_hlgs_acc);
    conf.sysControlParamsWltc[3] = GetLineEditValue(ui->lineEdit_hlgs_approachtime);
    conf.sysControlParamsWltc[4] = GetLineEditValue(ui->lineEdit_hlgs_accgain);
    conf.sysControlParamsWltc[5] = GetLineEditValue(ui->lineEdit_hlgs_advancedov);

    // APS
    conf.sysControlParamsWltc[6] = GetLineEditValue(ui->lineEdit_aps_advancedov);

    return true;
}
