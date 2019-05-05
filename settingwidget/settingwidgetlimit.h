#ifndef SETTINGWIDGETLIMIT_H
#define SETTINGWIDGETLIMIT_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetLimit;
}

class SettingWidgetLimit : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetLimit(QWidget *parent = 0);
    ~SettingWidgetLimit();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);

public slots:
    void SetToCurrentPosition();
    
private:
    Ui::SettingWidgetLimit *ui;

    QLineEdit* lineEdit_neg[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_pos[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETLIMIT_H
