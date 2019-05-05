#include "settingwidgetpedalrobotgetspeed.h"
#include "ui_settingwidgetpedalrobotgetspeed.h"

SettingWidgetPedalRobotGetSpeed::SettingWidgetPedalRobotGetSpeed(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotGetSpeed)
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;
}

SettingWidgetPedalRobotGetSpeed::~SettingWidgetPedalRobotGetSpeed()
{
    delete ui;
}

void SettingWidgetPedalRobotGetSpeed::LoadParameters(Configuration &conf)
{
    //测速方法
    ui->comboBox_getSpeedMethod->setCurrentIndex(conf.getSpeedMethod);
    ui->comboBox_calcSpeedErrorMethod->setCurrentIndex(conf.calcSpeedErrorMethod);

    //AD转换的校准系数
    SetLineEditValue(ui->lineEdit_AD_k, conf.ADCalibrateK);

    //脉冲转换方法
    ui->comboBox_selectLab->setCurrentIndex(conf.pulseSpeedCalibrateMethod);
    //脉冲校准系数
    SetLineEditValue(ui->lineEdit_calibratePulseFrequency, conf.pulseCalibrateFrequency);
    SetLineEditValue(ui->lineEdit_calibrateSpeed, conf.pulseCalibrateSpeed);
    SetLineEditValue(ui->lineEdit_pulseCounterSwitchSpeed, conf.pulseCounterSwitchSpeed);

    //程序用途
    ui->comboBox_pedalRobotUsage->setCurrentIndex(conf.pedalRobotUsage);
    //串口号
    ui->comboBox_serialDevice->setCurrentIndex(conf.serialDevice);
    //调试起始时间
    ui->lineEdit_pedalStartTimeS->setText(QString::number(conf.pedalStartTimeS));
}

bool SettingWidgetPedalRobotGetSpeed::StoreParameters(Configuration &conf)
{
    //测速方法
    conf.getSpeedMethod = ui->comboBox_getSpeedMethod->currentIndex();
    conf.calcSpeedErrorMethod = ui->comboBox_calcSpeedErrorMethod->currentIndex();

    //AD转换的校准系数
    conf.ADCalibrateK = GetLineEditValue(ui->lineEdit_AD_k);

    //脉冲转换方法
    conf.pulseSpeedCalibrateMethod = ui->comboBox_selectLab->currentIndex();
    //脉冲校准系数
    conf.pulseCalibrateFrequency = GetLineEditValue(ui->lineEdit_calibratePulseFrequency);
    conf.pulseCalibrateSpeed = GetLineEditValue(ui->lineEdit_calibrateSpeed);
    //脉冲测速的切换
    conf.pulseCounterSwitchSpeed = GetLineEditValue(ui->lineEdit_pulseCounterSwitchSpeed);

    //程序用途
    conf.pedalRobotUsage = ui->comboBox_pedalRobotUsage->currentIndex();
    //串口号
    conf.serialDevice = ui->comboBox_serialDevice->currentIndex();
    //调试起始时间
    conf.pedalStartTimeS = ui->lineEdit_pedalStartTimeS->text().toDouble();

    return true;
}

void SettingWidgetPedalRobotGetSpeed::DisplayVoltageWidgets(bool show)
{
    if(show){
        ui->groupBox_voltage2Speed->show();
    }else{
        ui->groupBox_voltage2Speed->hide();
    }
}

void SettingWidgetPedalRobotGetSpeed::DisplayPulseWidgets(bool show)
{
    if(show){
        ui->groupBox_pulse2Speed->show();
    }else{
        ui->groupBox_pulse2Speed->hide();
    }
}

void SettingWidgetPedalRobotGetSpeed::on_comboBox_selectLab_currentIndexChanged(int index)
{
    //不同的实验室(index) 对应不同的脉冲转换比例

    double pulseFreq = 0.0;
    double pulseSpeed = 0.0;
    switch (index){
    case 0://性能四驱 23.2KHZ对应100km/h
        pulseFreq = 23.2*1000.0;
        pulseSpeed = 100.0;
        break;
    case 1://排放四驱 32.63KHZ对应100km/h
        pulseFreq = 36.23*1000.0;
        pulseSpeed = 100.0;
        break;
    case 2://常温排放 3626HZ对应10km/h
        pulseFreq = 3626;
        pulseSpeed = 10.0;
        break;
    case 3://低温排放 3263HZ对应10km/h
        pulseFreq = 3263;
        pulseSpeed = 10.0;
        break;
    case 4://自定义
        pulseFreq = 1.0;
        pulseSpeed = 1.0;
    default:
        PRINTF(LOG_ERR, "%s: unknown index(%d)\n", __func__, index);
        break;
    }

    SetLineEditValue(ui->lineEdit_calibratePulseFrequency, pulseFreq);
    SetLineEditValue(ui->lineEdit_calibrateSpeed, pulseSpeed);
}

void SettingWidgetPedalRobotGetSpeed::on_comboBox_getSpeedMethod_currentIndexChanged(int index)
{
    switch (index){
    case 0://电压
        DisplayVoltageWidgets(true);
        DisplayPulseWidgets(false);
        break;
    case 1://脉冲
        DisplayVoltageWidgets(false);
        DisplayPulseWidgets(true);
        break;
    case 2://CAN
        DisplayVoltageWidgets(false);
        DisplayPulseWidgets(false);
        break;
    default:
        break;
    }
}
