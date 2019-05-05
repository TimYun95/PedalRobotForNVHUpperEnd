#include "settingwidgetpedalrobotnvhgetspeed.h"
#include "ui_settingwidgetpedalrobotnvhgetspeed.h"

SettingWidgetPedalRobotNVHGetSpeed::SettingWidgetPedalRobotNVHGetSpeed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotNVHGetSpeed)
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;
}

SettingWidgetPedalRobotNVHGetSpeed::~SettingWidgetPedalRobotNVHGetSpeed()
{
    delete ui;
}

void SettingWidgetPedalRobotNVHGetSpeed::LoadParameters(Configuration &conf)
{
    //串口号
    ui->comboBox_serialDevice->setCurrentIndex(conf.serialDevice);

    // O2V测试
    ui->comboBox_o2v->setCurrentIndex(conf.sysControlParams[0]);

    // 控制方式
    ui->comboBox_loop->setCurrentIndex(conf.sysControlParams[1]);

    //脉冲转换方法
    ui->comboBox_selectLab->setCurrentIndex(conf.pulseSpeedCalibrateMethod);
    //脉冲校准系数
    SetLineEditValue(ui->lineEdit_calibratePulseFrequency, conf.pulseCalibrateFrequency);
    SetLineEditValue(ui->lineEdit_calibrateSpeed, conf.pulseCalibrateSpeed);
}

bool SettingWidgetPedalRobotNVHGetSpeed::StoreParameters(Configuration &conf)
{
    //串口号
    conf.serialDevice = ui->comboBox_serialDevice->currentIndex();

    // O2V测试
    conf.sysControlParams[0] = ui->comboBox_o2v->currentIndex();

    // 控制方式
    conf.sysControlParams[1] = ui->comboBox_loop->currentIndex();

    //脉冲转换方法
    conf.pulseSpeedCalibrateMethod = ui->comboBox_selectLab->currentIndex();
    //脉冲校准系数
    conf.pulseCalibrateFrequency = GetLineEditValue(ui->lineEdit_calibratePulseFrequency);
    conf.pulseCalibrateSpeed = GetLineEditValue(ui->lineEdit_calibrateSpeed);

    return true;
}

void SettingWidgetPedalRobotNVHGetSpeed::on_comboBox_selectLab_currentIndexChanged(int index)
{
    Q_UNUSED(index);
}
