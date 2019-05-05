#ifndef SETTINGWIDGETPEDALROBOTSTUDYWLTC_H
#define SETTINGWIDGETPEDALROBOTSTUDYWLTC_H

#include <QWidget>
#include <QString>
#include <QLineEdit>

#include "settingbase.h"

namespace Ui {
class SettingWidgetPedalRobotStudyWltc;
}

class SettingWidgetPedalRobotStudyWltc : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetPedalRobotStudyWltc(QWidget *parent = 0);
    ~SettingWidgetPedalRobotStudyWltc();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

signals:
    void UpdateWltcParamsSignal();

private slots:
    void on_pushButton_loadCarTypeConfFile_clicked();

    void on_pushButton_saveCarTypeConfFile_clicked();

    void on_pushButton_readCalibratedParams_clicked();

private:
    void ReadCarTypeConfFile(const char* filePath);
    void SaveCarTypeConfFile(const char* filePath);

    void UpdateCarTypeDisplay();

private:
    Ui::SettingWidgetPedalRobotStudyWltc *ui;
    Configuration *myConf;

    const static int lineEditNum = 7;
    QLineEdit *mylineEdit[lineEditNum];

    const std::string parameterFilePath;
};

#endif // SETTINGWIDGETPEDALROBOTSTUDYWLTC_H
