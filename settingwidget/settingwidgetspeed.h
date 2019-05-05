#ifndef SETTINGWIDGETSPEED_H
#define SETTINGWIDGETSPEED_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetSpeed;
}

class SettingWidgetSpeed : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetSpeed(QWidget *parent = 0);
    ~SettingWidgetSpeed();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);
private:
    Ui::SettingWidgetSpeed *ui;
    QLineEdit* lineEdit_speedLimit[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_pulseFilter[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_motorAcceleration[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETSPEED_H
