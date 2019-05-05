#ifndef SETTINGWIDGETPEDALROBOTNVHDEATHZONE_H
#define SETTINGWIDGETPEDALROBOTNVHDEATHZONE_H

#include <QWidget>

#include "settingbase.h"

namespace Ui {
class SettingWidgetPedalRobotNVHDeathZone;
}

class SettingWidgetPedalRobotNVHDeathZone : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetPedalRobotNVHDeathZone(QWidget *parent = 0);
    ~SettingWidgetPedalRobotNVHDeathZone();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetPedalRobotNVHDeathZone *ui;
};

#endif // SETTINGWIDGETPEDALROBOTNVHDEATHZONE_H
