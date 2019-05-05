#ifndef MEDICALULTRASOUNDROBOT_H
#define MEDICALULTRASOUNDROBOT_H

/* 针对4DOF超声波医疗机器人
 * 使用了UI中的力控制
 * 使用AC6603的AD转换设备读取末端执行器处的力
 * 封装部分API
 * */

#include <QObject>

#include "configuration.h"

#include "device/ADConvertor/adcReader.h"

class MedicalUltrasoundRobot: public QObject
{
    Q_OBJECT//signals & slots
public:
    MedicalUltrasoundRobot(QObject *parent = 0);
    ~MedicalUltrasoundRobot();

    //AD转换卡的力控部分
    void EnableForceControl(bool checked);
    void CheckADForce();
    int CheckADButton();

    //伺服电机的ALARM信号部分
    void ResetAlarmSignal();
    void CheckServoAlarmSignal(const int inStatus);

signals:
    void ForceValueSignal(QString);

public slots:
    void ForceControlClickedSlot(bool checked);

private:
    void SendForceValue();
    void LargeForceAlarm();
    void StopRobot();

private:
    Configuration *conf;

    //AD转换卡的力控部分
    adcReader* adc;
    double forceValue;
    bool isForceControlMode;

    //力传感器 力过大报警
    int largeForceCount;
    //伺服电机的ALARM信号部分
    int AlarmCount;//计数 防止误判

    //伺服回原后的处理
    bool isGoingHome;//回原+返回运动学零点的过程
};

#endif // MEDICALULTRASOUNDROBOT_H
