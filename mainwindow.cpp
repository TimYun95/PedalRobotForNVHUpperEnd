#include "mainwindow.h"

#include <QTimer>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>

#include "common/robotparams.h"
#include "common/globalvariables.h"

MainWindow::MainWindow(QWidget *parent) :
    QWidget(parent)
{
    this->setWindowTitle(QObject::tr(RobotParams::robotType));
    this->setWindowState(Qt::WindowMaximized);

#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
    m_Uri = new PedalRobotUI(this);
#else
    m_Uri = new URI(this, true, true);
#endif

    timerMainWindow=new QTimer(this);
    connect(timerMainWindow,SIGNAL(timeout()),this,SLOT(MyTimerDone()));
    timerMainWindow->start(100);

    statusLabel = new QLabel();
    QFont ft;
    ft.setPointSize(20);
    statusLabel->setFont(ft);

    /* 布局 */
    vLayout =new QVBoxLayout();
    vLayout->addWidget(m_Uri);
    hLayout=new QHBoxLayout();
    hLayout->addWidget(statusLabel);
    hLayout->addStretch(1);
    vLayout->addLayout(hLayout);
    this->setLayout(vLayout);
}

MainWindow::~MainWindow()
{
    timerMainWindow->stop();
    delete hLayout;
    delete vLayout;
    delete statusLabel;
    delete timerMainWindow;
    delete m_Uri;
}

void MainWindow::MyTimerDone()
{
    statusLabel->setText(StatusString::Instance()->GetStatusString());
}
