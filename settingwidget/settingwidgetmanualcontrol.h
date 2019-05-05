#ifndef SETTINGWIDGETMANUALCONTROL_H
#define SETTINGWIDGETMANUALCONTROL_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"

namespace Ui {
class SettingWidgetManualControl;
}

class SettingWidgetManualControl : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetManualControl(QWidget *parent = 0);
    ~SettingWidgetManualControl();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetManualControl *ui;

    QLineEdit* lineEdit_speedRatio[RobotParams::UIdof];
};

#endif // SETTINGWIDGETMANUALCONTROL_H
