#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QWidget>

#ifdef ENABLE_PEDAL_ROBOT_WIDGETS//使用其独立的UI界面
 #include "robottype/pedalrobot/pedalrobotui.h"
#else
 #include "uri.h"
#endif

class QTimer;
class QLabel;
class QVBoxLayout;
class QHBoxLayout;

class MainWindow : public QWidget
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();
    
signals:
    
private slots:
    void MyTimerDone();

private:
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
    PedalRobotUI *m_Uri;
#else
    URI* m_Uri;
#endif

    QTimer* timerMainWindow;
    QLabel* statusLabel;//左下角状态

    QVBoxLayout* vLayout;
    QHBoxLayout* hLayout;
};

#endif // MAINWINDOW_H
