#ifndef SETTINGWIDGETSINGLEAXIS_H
#define SETTINGWIDGETSINGLEAXIS_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"

namespace Ui {
class SettingWidgetSingleAxis;
}

class SettingWidgetSingleAxis : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetSingleAxis(QWidget *parent = 0);
    ~SettingWidgetSingleAxis();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    Ui::SettingWidgetSingleAxis *ui;

    QLineEdit* lineEdit_btnRatio[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_sliderRatio[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETSINGLEAXIS_H
