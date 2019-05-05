#ifndef PEDALROBOTUSBPROCESSOR_H
#define PEDALROBOTUSBPROCESSOR_H

#include <QObject>

#include "uri.h"

#include "device/usbcan/cananalyst.h"
#include "device/MP412/mp412device.h"

class PedalRobotUSBProcessor : public QObject
{
    Q_OBJECT
public:
    explicit PedalRobotUSBProcessor(QObject *parent = 0);
    ~PedalRobotUSBProcessor();

public: // 对外函数
    void UpdateDeviceReady(); // 准备设备数据
    void UpdateDeviceControl(); // 更新设备数据

    double GetBrakeOV(); // 返回刹车开度
    double GetAcceleratorOV(); // 返回油门开度
    double GetCanCarSpeed(); // 返回CAN车速
    int GetPowerMode(); // 返回车辆状态
    double GetMP412CarSpeed(); // 返回PULSE车速
    double GetAdVoltage(); // 返回AD电压
    double GetPulseFrequency(); // 返回PULSE频率

private: // 内部函数
    void UpdateCANData(); // 更新CAN数据
    void UpdateMP412Data(); // 更新脉冲数据

signals: // 信号

private slots: // 内部槽函数

public: // 对外变量

private: // 内部变量
    CANAnalyst *m_canAnalyst; // CAN数据采集
    MP412Device *myMP412; // 脉冲数据采集

    double m_brakeOpenValue; // CAN刹车开度
    double m_accOpenValue; // CAN油门开度
    double m_usbCanCarSpeed; // CAN车速
    int m_powerMode; // CAN车辆运行状态
    double m_usbMP412CarSpeed; // MP412转换的车速信号
};

#endif // PEDALROBOTUSBPROCESSOR_H
