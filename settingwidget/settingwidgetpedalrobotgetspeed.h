#ifndef SETTINGWIDGETPEDALROBOTGETSPEED_H
#define SETTINGWIDGETPEDALROBOTGETSPEED_H

#include <QWidget>

#include "settingbase.h"

namespace Ui {
class SettingWidgetPedalRobotGetSpeed;
}

class SettingWidgetPedalRobotGetSpeed : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetPedalRobotGetSpeed(QWidget *parent = 0);
    ~SettingWidgetPedalRobotGetSpeed();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

    //和comboBox_selectLab的内容顺序一致
    enum tagGetSpeedMethod{
        Voltage2Speed=0,
        PulseFrequency2Speed,
        CANSpeed
    };

    //和comboBox_pedalRobotUsage的内容顺序一致
    enum tagPedalRobotUsage{
        NedcControl=0,
        WltcControl,
        SampleOpenValue,
        ManualEncoderControl
    };

private:
    void DisplayVoltageWidgets(bool show);
    void DisplayPulseWidgets(bool show);

private slots:
    void on_comboBox_selectLab_currentIndexChanged(int index);

    void on_comboBox_getSpeedMethod_currentIndexChanged(int index);

private:
    Ui::SettingWidgetPedalRobotGetSpeed *ui;
};

#endif // SETTINGWIDGETPEDALROBOTGETSPEED_H
