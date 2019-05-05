#ifndef SETTINGWIDGETERRORLIMIT_H
#define SETTINGWIDGETERRORLIMIT_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetErrorLimit;
}

class SettingWidgetErrorLimit : public QWidget,public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetErrorLimit(QWidget *parent = 0);
    ~SettingWidgetErrorLimit();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetErrorLimit *ui;
    QLineEdit* lineEdit_errorLimitTrans[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_errorLimitWork[RobotParams::UIdof];
    bool CheckValidity();
};

#endif // SETTINGWIDGETERRORLIMIT_H
