#ifndef SETTINGWIDGETABSVALUEATORIGIN_H
#define SETTINGWIDGETABSVALUEATORIGIN_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetAbsValueAtOrigin;
}

class SettingWidgetAbsValueAtOrigin : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetAbsValueAtOrigin(QWidget *parent = 0);
    ~SettingWidgetAbsValueAtOrigin();
    
    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);

private:
    Ui::SettingWidgetAbsValueAtOrigin *ui;
    QLineEdit* lineEdit_singleAbsValue[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_multiAbsValue[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETABSVALUEATORIGIN_H
