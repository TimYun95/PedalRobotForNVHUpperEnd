#ifndef SETTINGWIDGETMOTOR_H
#define SETTINGWIDGETMOTOR_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetMotor;
}

class SettingWidgetMotor : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetMotor(QWidget *parent = 0);
    ~SettingWidgetMotor();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);
    
private:
    Ui::SettingWidgetMotor *ui;
    QLineEdit* lineEdit_motorRatio[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_motorSubdivision[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETMOTOR_H
