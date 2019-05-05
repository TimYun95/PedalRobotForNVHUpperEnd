#ifndef SETTINGWIDGETFORCECONTROL_H
#define SETTINGWIDGETFORCECONTROL_H

#include <QWidget>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetForceControl;
}

class SettingWidgetForceControl : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetForceControl(QWidget *parent = 0);
    ~SettingWidgetForceControl();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

signals:
    void ForceControlClickedSignal(bool checked);

private slots:
    void on_pushButton_forceControl_clicked();

public slots:
    void ForceValueSlot(QString);

private:
    Ui::SettingWidgetForceControl *ui;
};

#endif // SETTINGWIDGETFORCECONTROL_H
