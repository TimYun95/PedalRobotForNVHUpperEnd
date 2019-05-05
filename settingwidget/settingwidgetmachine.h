#ifndef SETTINGWIDGETMACHINE_H
#define SETTINGWIDGETMACHINE_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetMachine;
}

class SettingWidgetMachine : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetMachine(QWidget *parent = 0);
    ~SettingWidgetMachine();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetMachine *ui;
    QLineEdit* lineEdit_homeAngle[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETMACHINE_H
