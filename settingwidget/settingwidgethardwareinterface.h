#ifndef SETTINGWIDGETHARDWAREINTERFACE_H
#define SETTINGWIDGETHARDWAREINTERFACE_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetHardwareInterface;
}

class SettingWidgetHardwareInterface : public QWidget,public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetHardwareInterface(QWidget *parent = 0);
    ~SettingWidgetHardwareInterface();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetHardwareInterface *ui;
    QLineEdit* lineEdit_motorPort[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_encoderPort[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_optSwitchPort[RobotParams::UIAxisNum];
    bool CheckValidityOfParameters();
};

#endif // SETTINGWIDGETHARDWAREINTERFACE_H
