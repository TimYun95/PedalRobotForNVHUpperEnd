#ifndef SETTINGWIDGETPID_H
#define SETTINGWIDGETPID_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetPID;
}

class SettingWidgetPID : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetPID(QWidget *parent = 0);
    ~SettingWidgetPID();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);
    
private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::SettingWidgetPID *ui;
    QLineEdit* lineEdit_kp[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_ki[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_kd[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_kf[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_kp_work[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_ki_work[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_kd_work[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_kf_work[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETPID_H
