#ifndef SETTINGWIDGETMACHINEPARAMS_H
#define SETTINGWIDGETMACHINEPARAMS_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>

#include "settingbase.h"

namespace Ui {
class SettingWidgetMachineParams;
}

/* 预留machineParamNum个参数 用于保存机器人运动学的相关参数 */
class SettingWidgetMachineParams : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetMachineParams(QWidget *parent = 0);
    ~SettingWidgetMachineParams();
    
    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    void InitLabel();
    void InitLineEdit();
    void HideMachineParamsWidget(int startIndex, int endIndex);

    void InitDifferentRobot();
    void InitDof4Medical();
    void InitDof2Pedal();
    void InitDof5ScaraPaint();
    void InitDof5IgusFlexibleShaft();
    void InitDof6XyzVesselPuncture();

private:
    Ui::SettingWidgetMachineParams *ui;
    QLabel *label_machineParams[RobotParams::machineParamNum];
    QLineEdit* lineEdit_machineParams[RobotParams::machineParamNum];
};

#endif // SETTINGWIDGETMACHINEPARAMS_H
