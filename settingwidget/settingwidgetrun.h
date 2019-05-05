#ifndef SETTINGWIDGETRUN_H
#define SETTINGWIDGETRUN_H

#include <QWidget>

#include "settingbase.h"

namespace Ui {
class SettingWidgetRun;
}

class SettingWidgetRun : public QWidget, public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetRun(QWidget *parent = 0);
    ~SettingWidgetRun();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);
    
private slots:
    void on_pushButton_autoSetSerialPort_clicked();

private:
    Ui::SettingWidgetRun *ui;
};

#endif // SETTINGWIDGETRUN_H
