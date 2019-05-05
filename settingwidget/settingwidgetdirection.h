#ifndef SETTINGWIDGETDIRECTION_H
#define SETTINGWIDGETDIRECTION_H

#include <QWidget>
#include <QSlider>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetDirection;
}

class SettingWidgetDirection : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetDirection(QWidget *parent = 0);
    ~SettingWidgetDirection();
    
    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private slots:
    void on_comboBox_currentIndexChanged(int index);

private:
    Ui::SettingWidgetDirection *ui;
    QSlider *horizontalSlider_motor[RobotParams::UIAxisNum];
    QSlider *horizontalSlider_encoder[RobotParams::UIAxisNum];
    QSlider *horizontalSlider_goHome[RobotParams::UIAxisNum];

    void ShowSliders_Motor(bool flag);
    void ShowSliders_Encoder(bool flag);
    void ShowSliders_GoHome(bool flag);
};

#endif // SETTINGWIDGETDIRECTION_H
