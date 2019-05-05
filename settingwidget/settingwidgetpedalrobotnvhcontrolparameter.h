#ifndef SETTINGWIDGETPEDALROBOTNVHCONTROLPARAMETER_H
#define SETTINGWIDGETPEDALROBOTNVHCONTROLPARAMETER_H

#include <QWidget>

#include "settingbase.h"

namespace Ui {
class SettingWidgetPedalRobotNVHControlParameter;
}

class SettingWidgetPedalRobotNVHControlParameter : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetPedalRobotNVHControlParameter(QWidget *parent = 0);
    ~SettingWidgetPedalRobotNVHControlParameter();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetPedalRobotNVHControlParameter *ui;
};

#endif // SETTINGWIDGETPEDALROBOTNVHCONTROLPARAMETER_H
