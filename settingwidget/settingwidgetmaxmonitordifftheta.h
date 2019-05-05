#ifndef SETTINGWIDGETMAXMONITORDIFFTHETA_H
#define SETTINGWIDGETMAXMONITORDIFFTHETA_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetMaxMonitorDiffTheta;
}

class SettingWidgetMaxMonitorDiffTheta : public QWidget,public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetMaxMonitorDiffTheta(QWidget *parent = 0);
    ~SettingWidgetMaxMonitorDiffTheta();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);

private:
    Ui::SettingWidgetMaxMonitorDiffTheta *ui;
    QLineEdit* lineEdit_maxMonitorDiffTheta[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETMAXMONITORDIFFTHETA_H
