#include "settingwidgetrun.h"
#include "ui_settingwidgetrun.h"

#include <QtSerialPort/QSerialPortInfo>

SettingWidgetRun::SettingWidgetRun(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetRun)
{
    ui->setupUi(this);

#ifdef ENABLE_PEDAL_ROBOT_WIDGETS//对于踏板机器人 该设置移至速度页面
    ui->comboBox_serialDevice->hide();
    ui->label_serialDeivce->hide();
    ui->pushButton_autoSetSerialPort->hide();
#endif

    for(int portIndex=0; portIndex<30; ++portIndex){
#ifdef WIN32
        QString portName = "COM" + QString::number(portIndex + 1);//0=COM1
#else
        QString portName = "ttyUSB" + QString::number(portIndex);//0=ttyUSB0
#endif

        ui->comboBox_serialDevice->addItem(portName);
    }
}

SettingWidgetRun::~SettingWidgetRun()
{
    delete ui;
}

void SettingWidgetRun::LoadParameters(Configuration &conf)
{
    ui->comboBox_serialDevice->setCurrentIndex(conf.serialDevice);
    SetLineEditValue(ui->lineEdit_motorMaxPps, conf.motorMaxPps);
    SetLineEditValue(ui->lineEdit_angleMutationThreshold, conf.angleMutationThreshold);

    SetLineEditValue(ui->lineEdit_openGunDelay, conf.openGunDelay);
    SetLineEditValue(ui->lineEdit_closeGunDelay, conf.closeGunDelay);

    SetLineEditValue(ui->lineEdit_transSpeed, conf.transSpeed);
    SetLineEditValue(ui->lineEdit_calibratingPoints, conf.calibratingPoints);
    SetLineEditValue(ui->lineEdit_transPointRatio, conf.transPointRatio);
}

bool SettingWidgetRun::StoreParameters(Configuration &conf)
{
#ifndef ENABLE_PEDAL_ROBOT_WIDGETS
    conf.serialDevice=ui->comboBox_serialDevice->currentIndex();
#endif
    conf.motorMaxPps = GetLineEditValue(ui->lineEdit_motorMaxPps);
    conf.angleMutationThreshold = GetLineEditValue(ui->lineEdit_angleMutationThreshold);

    conf.openGunDelay = GetLineEditValue(ui->lineEdit_openGunDelay);
    conf.closeGunDelay = GetLineEditValue(ui->lineEdit_closeGunDelay);

    conf.transSpeed = GetLineEditValue(ui->lineEdit_transSpeed);
    conf.calibratingPoints = GetLineEditValue(ui->lineEdit_calibratingPoints);
    conf.transPointRatio = GetLineEditValue(ui->lineEdit_transPointRatio);

    return true;
}

void SettingWidgetRun::on_pushButton_autoSetSerialPort_clicked()
{
    const QList<QSerialPortInfo>& portList = QSerialPortInfo::availablePorts();
    PRINTF(LOG_DEBUG, "%s: find %d available serial port(s).\n", __func__, portList.size());

#ifdef WIN32
    QString portName("COM1");
    for(int i=0; i<portList.size(); ++i){
        const QString &name = portList[i].portName();
        if(name != "COM1"){//COM1是主板上的串口 不是USB转串口的COM口
            portName = name;//第一个非COM1的端口
            break;
        }
    }
    int portNum = portName.remove("COM").toInt();
    ui->comboBox_serialDevice->setCurrentIndex(portNum-1);
#else
    QString portName("/dev/ttyUSB0");
    for(int i=0; i<portList.size(); ++i){
        const QString &name = portList[i].portName();
        if(name != "/dev/ttyS0"){//ttyS0是主板上的串口 不是USB转串口的COM口
            portName = name;//第一个非ttyS0的端口
            break;
        }
    }
    int portIndex = portName.remove("/dev/ttyUSB").toInt();
    ui->comboBox_serialDevice->setCurrentIndex(portIndex);
#endif
}
