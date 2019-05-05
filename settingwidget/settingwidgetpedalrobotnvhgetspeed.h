#ifndef SETTINGWIDGETPEDALROBOTNVHGETSPEED_H
#define SETTINGWIDGETPEDALROBOTNVHGETSPEED_H

#include <QWidget>

#include "settingbase.h"

namespace Ui {
class SettingWidgetPedalRobotNVHGetSpeed;
}

class SettingWidgetPedalRobotNVHGetSpeed : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetPedalRobotNVHGetSpeed(QWidget *parent = 0);
    ~SettingWidgetPedalRobotNVHGetSpeed();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private slots:
    void on_comboBox_selectLab_currentIndexChanged(int index);

private:
    Ui::SettingWidgetPedalRobotNVHGetSpeed *ui;
};

#endif // SETTINGWIDGETPEDALROBOTNVHGETSPEED_H
