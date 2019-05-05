#ifndef SETTINGWIDGETGOHOMESPEED_H
#define SETTINGWIDGETGOHOMESPEED_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetGoHomeSpeed;
}

class SettingWidgetGoHomeSpeed : public QWidget,public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetGoHomeSpeed(QWidget *parent = 0);
    ~SettingWidgetGoHomeSpeed();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);

private:
    Ui::SettingWidgetGoHomeSpeed *ui;
    QLineEdit* lineEdit_highSpeed[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_lowSpeed[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETGOHOMESPEED_H
