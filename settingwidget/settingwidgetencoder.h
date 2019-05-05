#ifndef SETTINGWIDGETENCODER_H
#define SETTINGWIDGETENCODER_H

#include <QWidget>
#include <QLineEdit>

#include "settingbase.h"
#include "common/robotparams.h"

namespace Ui {
class SettingWidgetEncoder;
}

class SettingWidgetEncoder : public QWidget,public SettingBase
{
    Q_OBJECT
    
public:
    explicit SettingWidgetEncoder(QWidget *parent = 0);
    ~SettingWidgetEncoder();

    virtual void LoadParameters(Configuration &);
    virtual bool StoreParameters(Configuration &);
    
private:
    Ui::SettingWidgetEncoder *ui;
    QLineEdit* lineEdit_encoderRatio[RobotParams::UIAxisNum];
    QLineEdit* lineEdit_encoderSubdivision[RobotParams::UIAxisNum];
};

#endif // SETTINGWIDGETENCODER_H
