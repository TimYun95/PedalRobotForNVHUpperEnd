#ifndef SETTINGWIDGETWEBSOCKET_H
#define SETTINGWIDGETWEBSOCKET_H

#include <QWidget>

#include "settingbase.h"

namespace Ui {
class SettingWidgetWebSocket;
}

class SettingWidgetWebSocket : public QWidget, public SettingBase
{
    Q_OBJECT

public:
    explicit SettingWidgetWebSocket(QWidget *parent = 0);
    ~SettingWidgetWebSocket();

    virtual void LoadParameters(Configuration &conf);
    virtual bool StoreParameters(Configuration &conf);

private:
    bool CheckStringLen(const std::string& str);
    bool DisplayErrorWarningBox(const std::string& str);

private slots:
    void on_pushButton_browse_clicked();

private:
    Ui::SettingWidgetWebSocket *ui;
};

#endif // SETTINGWIDGETWEBSOCKET_H
