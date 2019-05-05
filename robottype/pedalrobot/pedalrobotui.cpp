#include "pedalrobotui.h"
#include "ui_pedalrobotui.h"

#include <QMetaType>

#include "mywidget/mymessagebox.h"
#include "common/printf.h"

#include "fileoperation/normalfile.h"
#include "assistantfunc/fileassistantfunc.h"

/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- 基本函数 -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
PedalRobotUI::PedalRobotUI(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::PedalRobotUI),
    generalArrayLength(190),
    maxShowTime(12.0),
    maxShowSpeed(145.0),
    maxShowOpenValue(105.0),
    plotMultiplier(10), plotCount(0)
{
    ui->setupUi(this);

    // 初始化NVH控制类
    pdNVH = new PedalRobotNVH();

    // 首次初始化URI
    pUri = pdNVH->GetURI();
    ui->tabWidget->addTab((QWidget*)pUri, QObject::tr(" 设置 "));

    InitWidgets(); // 界面初始化
    InitSignalSlot(); // 信号槽初始化

    pdNVH->moveToThread(&pdThread);
    pdThread.setPriority(QThread::TimeCriticalPriority);
    connect(&pdThread, SIGNAL(finished()), pdNVH, SLOT(deleteLater()), Qt::QueuedConnection);
    pdThread.start();
    pdNVH->StartInternalTimer(); // 开始内部定时
}

PedalRobotUI::~PedalRobotUI()
{
    pdNVH->StopInternalTimer();
    EndThread();
    delete ui;
}

/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- 对外函数 -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/





/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- 内部函数 -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
void PedalRobotUI::InitWidgets()
{
//#ifdef ENABLE_BRAKE_NVH
//    ui->frame_brk_head->setVisible(true);
//    ui->frame_brk_info->setVisible(true);
//    ui->frame_brk_ov->setVisible(true);
//#else
    ui->frame_brk_head->setVisible(false);
    ui->frame_brk_info->setVisible(false);
    ui->frame_brk_ov->setVisible(false);
//#endif

    InitCurvePlot();

    EnableButtonsForGoHome(false);

    ui->lineEdit_dasfp_err->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_dasfp_errpercent->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_dasct_err->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_dasct_errpercent->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_cs_err->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_cs_errpercent->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_pgs_err->setStyleSheet("background-color: green; color: white");
    ui->lineEdit_pgs_errpercent->setStyleSheet("background-color: green; color: white");

    ui->tabWidget_nvh->setTabEnabled(6, false);
    ui->tabWidget_nvh->setStyleSheet("QTabBar::tab:last{background-color: green; color: white}");
}

void PedalRobotUI::InitCurvePlot()
{
    // 初始化绘图数组
    speedTimeActual.clear();
    speedTimeActual.reserve(generalArrayLength);
    speedActual.clear();
    speedActual.reserve(generalArrayLength);
    speedTimeTarget.clear();
    speedTimeTarget.reserve(generalArrayLength);
    speedLowerBound.clear();
    speedLowerBound.reserve(generalArrayLength);
    speedUpperBound.clear();
    speedUpperBound.reserve(generalArrayLength);
    speedTarget.clear();
    speedTarget.reserve(generalArrayLength);

    openValueTimeActual.clear();
    openValueTimeActual.reserve(generalArrayLength);
    openValueActual.clear();
    openValueActual.reserve(generalArrayLength);
    openValueTimeTarget.clear();
    openValueTimeTarget.reserve(generalArrayLength);
    openValueLowerBound.clear();
    openValueLowerBound.reserve(generalArrayLength);
    openValueUpperBound.clear();
    openValueUpperBound.reserve(generalArrayLength);
    openValueTarget.clear();
    openValueTarget.reserve(generalArrayLength);

    ui->curvePlot_speed->clearGraphs();
    ui->curvePlot_ov->clearGraphs();

    ui->curvePlot_speed->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->curvePlot_speed->xAxis->setLabel(QObject::tr("时间(s)"));
    ui->curvePlot_speed->yAxis->setLabel(QObject::tr("速度(km/h)"));

    ui->curvePlot_ov->setInteractions(QCP::iRangeDrag | QCP::iRangeZoom | QCP::iSelectAxes | QCP::iSelectLegend | QCP::iSelectPlottables);
    ui->curvePlot_ov->xAxis->setLabel(QObject::tr("时间(s)"));
    ui->curvePlot_ov->yAxis->setLabel(QObject::tr("油门开度(%)"));

    ui->curvePlot_speed->addGraph();
    ui->curvePlot_speed->addGraph();
    ui->curvePlot_speed->addGraph();
    ui->curvePlot_speed->addGraph();

    ui->curvePlot_ov->addGraph();
    ui->curvePlot_ov->addGraph();
    ui->curvePlot_ov->addGraph();
    ui->curvePlot_ov->addGraph();

    QPen pen;
    pen.setWidth(3);
    pen.setColor(Qt::green); // 目标曲线
    ui->curvePlot_speed->graph(TargetCurveLayout)->setPen(pen);
    ui->curvePlot_ov->graph(TargetCurveLayout)->setPen(pen);
    pen.setColor(Qt::red); // 实际曲线
    ui->curvePlot_speed->graph(ActionCurveLayout)->setPen(pen);
    ui->curvePlot_ov->graph(ActionCurveLayout)->setPen(pen);
    pen.setWidth(1);
    pen.setColor(Qt::blue); // 上下界
    ui->curvePlot_speed->graph(UpperCurveLayout)->setPen(pen);
    ui->curvePlot_speed->graph(LowerCurveLayout)->setPen(pen);
    ui->curvePlot_ov->graph(UpperCurveLayout)->setPen(pen);
    ui->curvePlot_ov->graph(LowerCurveLayout)->setPen(pen);

    ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(false);
    ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(false);
    ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(false);
    ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(false);
    ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
    ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

    ui->curvePlot_speed->xAxis->setRange(0.0, maxShowTime);
    ui->curvePlot_speed->yAxis->setRange(0.0, maxShowSpeed);
    ui->curvePlot_ov->xAxis->setRange(0.0, maxShowTime);
    ui->curvePlot_ov->yAxis->setRange(0.0, maxShowOpenValue);

    QFont font;
    font.setPointSize(15);
    ui->curvePlot_speed->xAxis->setLabelFont(font);
    ui->curvePlot_speed->yAxis->setLabelFont(font);
    ui->curvePlot_ov->xAxis->setLabelFont(font);
    ui->curvePlot_ov->yAxis->setLabelFont(font);
    ui->curvePlot_speed->xAxis->setTickLabelFont(font);
    ui->curvePlot_speed->yAxis->setTickLabelFont(font);
    ui->curvePlot_ov->xAxis->setTickLabelFont(font);
    ui->curvePlot_ov->yAxis->setTickLabelFont(font);

    currentNVH = General;
    firstToGeneral = true;
    ifPlotEnable = true;
}

void PedalRobotUI::InitSignalSlot()
{
    connect(pUri, SIGNAL(Signal_WidgetShowInformMessageBox(QString)), this, SLOT(Slot_WidgetShowInformMessageBox(QString)), Qt::QueuedConnection);

    connect(ui->pushButton_plus1, SIGNAL(pressed()), this, SLOT(Slot_SingleAxisPressed()));
    connect(ui->pushButton_plus2, SIGNAL(pressed()), this, SLOT(Slot_SingleAxisPressed()));
    connect(ui->pushButton_minus1, SIGNAL(pressed()), this, SLOT(Slot_SingleAxisPressed()));
    connect(ui->pushButton_minus2, SIGNAL(pressed()), this, SLOT(Slot_SingleAxisPressed()));
    connect(ui->pushButton_plus1, SIGNAL(released()), this, SLOT(Slot_SingleAxisReleased()));
    connect(ui->pushButton_plus2, SIGNAL(released()), this, SLOT(Slot_SingleAxisReleased()));
    connect(ui->pushButton_minus1, SIGNAL(released()), this, SLOT(Slot_SingleAxisReleased()));
    connect(ui->pushButton_minus2, SIGNAL(released()), this, SLOT(Slot_SingleAxisReleased()));

    connect(ui->radioButton_dasfp_vl, SIGNAL(clicked(bool)), this, SLOT(Slot_NVH_DASFP_RadioBtnClicked(bool)));
    connect(ui->radioButton_dasfp_l, SIGNAL(clicked(bool)), this, SLOT(Slot_NVH_DASFP_RadioBtnClicked(bool)));
    connect(ui->radioButton_dasfp_m, SIGNAL(clicked(bool)), this, SLOT(Slot_NVH_DASFP_RadioBtnClicked(bool)));

    connect(ui->pushButton_hlgs_slowplus, SIGNAL(pressed()), this, SLOT(Slot_NVH_HLGS_SlowRunPressed()));
    connect(ui->pushButton_hlgs_slowdecline, SIGNAL(pressed()), this, SLOT(Slot_NVH_HLGS_SlowRunPressed()));
    connect(ui->pushButton_hlgs_slowplus, SIGNAL(released()), this, SLOT(Slot_NVH_HLGS_SlowRunReleased()));
    connect(ui->pushButton_hlgs_slowdecline, SIGNAL(released()), this, SLOT(Slot_NVH_HLGS_SlowRunReleased()));

    connect(this, SIGNAL(Signal_SingleAxisMove(int,int)), pdNVH, SLOT(Slot_SingleAxisMove(int,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_EnterIdle()), pdNVH, SLOT(Slot_EnterIdle()), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_GoHome()), pdNVH, SLOT(Slot_GoHome()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_SoftStop()), pdNVH, SLOT(Slot_SoftStop()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_SoftStopAndLift()), pdNVH, SLOT(Slot_SoftStopAndLift()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_BeginPreTest(double,double,bool,double,bool,double,bool,int,double,double,bool,bool,int,bool,int)), pdNVH, SLOT(Slot_BeginPreTest(double,double,bool,double,bool,double,bool,int,double,double,bool,bool,int,bool,int)), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_StopPreTestRightNow()), pdNVH, SLOT(Slot_StopPreTestRightNow()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_ReadPreTestFile(QString,bool)), pdNVH, SLOT(Slot_ReadPreTestFile(QString,bool)), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_ContinueO2VTest()), pdNVH, SLOT(Slot_ContinueO2VTest()), Qt::QueuedConnection);

    qRegisterMetaType<paraMap>("paraMap");
    connect(pdNVH, SIGNAL(Signal_UpdateWidget(paraMap,int)), this, SLOT(Slot_UpdateWidget(paraMap,int)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_UpdateWidgetForPreTestFinished()), this, SLOT(Slot_UpdateWidgetForPreTestFinished()), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_UpdateWidgetForO2VWaitBegin(bool)), this, SLOT(Slot_UpdateWidgetForO2VWaitBegin(bool)), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_NVH_DASFP_GetRecommandOV(int)), pdNVH, SLOT(Slot_NVH_DASFP_GetRecommandOV(int)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_DASFP_UpdateWidgetForRecommandOV(double)), this, SLOT(Slot_NVH_DASFP_UpdateWidgetForRecommandOV(double)), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_NVH_DASFP_Start(double,int)), pdNVH, SLOT(Slot_NVH_DASFP_Start(double,int)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_DASFP_Finished(int,double,double)), this, SLOT(Slot_NVH_DASFP_Finished(int,double,double)), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_NVH_DASCT_Start(double,double,int)), pdNVH, SLOT(Slot_NVH_DASCT_Start(double,double,int)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_DASCT_Finished(int)), this, SLOT(Slot_NVH_DASCT_Finished(int)), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_NVH_CS_TestOV(double)), pdNVH, SLOT(Slot_NVH_CS_TestOV(double)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_CS_TestOVFinished()), this, SLOT(Slot_NVH_CS_TestOVFinished()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_NVH_CS_Start(double,double)), pdNVH, SLOT(Slot_NVH_CS_Start(double,double)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_CS_Finished(int)), this, SLOT(Slot_NVH_CS_Finished(int)), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_NVH_HLGS_KeepSpeed(double,double,double,double)), pdNVH, SLOT(Slot_NVH_HLGS_KeepSpeed(double,double,double,double)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_HLGS_KeepSpeedFinished()), this, SLOT(Slot_NVH_HLGS_KeepSpeedFinished()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_NVH_HLGS_SingleAxisSlowMove(bool)), pdNVH, SLOT(Slot_NVH_HLGS_SingleAxisSlowMove(bool)), Qt::QueuedConnection);
    connect(this,SIGNAL(Signal_NVH_HLGS_Start(double,double,double,double)), pdNVH, SLOT(Slot_NVH_HLGS_Start(double,double,double,double)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_HLGS_Finished(int)), this, SLOT(Slot_NVH_HLGS_Finished(int)), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_NVH_PGS_Start(double)), pdNVH, SLOT(Slot_NVH_PGS_Start(double)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_PGS_Finished(int)), this, SLOT(Slot_NVH_PGS_Finished(int)), Qt::QueuedConnection);

    connect(this,SIGNAL(Signal_NVH_APS_Start(double,double,double,double)), pdNVH, SLOT(Slot_NVH_APS_Start(double,double,double,double)), Qt::QueuedConnection);
    connect(pdNVH, SIGNAL(Signal_NVH_APS_Finished(int)), this, SLOT(Slot_NVH_APS_Finished(int)), Qt::QueuedConnection);

    connect(this, SIGNAL(Signal_NVH_ConfirmStop()), pdNVH, SLOT(Slot_NVH_ConfirmStop()), Qt::QueuedConnection);
    connect(this, SIGNAL(Signal_NVH_StopNow()), pdNVH, SLOT(Slot_NVH_StopNow()), Qt::QueuedConnection);
}

void PedalRobotUI::EndThread()
{
    pdThread.quit();
    pdThread.wait();
}

void PedalRobotUI::EnableButtonsForGoHome(bool enable)
{
    //DEBUG模式 不需要按键的使能控制
    if(Configuration::Instance()->machineParam[6] == 1.0){
        return;
    }

    //enable=true 回原完成
    ui->tabWidget_nvh->setEnabled(enable);
    ui->pushButton_softStop_liftPedals->setEnabled(enable);
    ui->pushButton_pretest->setEnabled(enable);
    ifPlotEnable = enable;
}

void PedalRobotUI::EnableButtonsForSingle(bool enable)
{
    ui->pushButton_plus1->setEnabled(enable);
    ui->pushButton_plus2->setEnabled(enable);
    ui->pushButton_minus1->setEnabled(enable);
    ui->pushButton_minus2->setEnabled(enable);
}

void PedalRobotUI::UpdateNVHWidget(const paraMap& dataInfo)
{
    if (ui->tabWidget_nvh->currentIndex() == 4 && currentNVH == PassingGearSweep)
    {
        double aimOV = ui->comboBox_pgs_aimov->currentText().trimmed().toDouble();

        double errOV = dataInfo["accOV"] - aimOV;
        double errOVP = errOV / aimOV;
        ui->lineEdit_pgs_err->setText(QString::number(errOV, 'f', 1));
        ui->lineEdit_pgs_errpercent->setText(std::isnan(errOVP) ? "---" : QString::number(errOVP * 100.0, 'f', 1));

        if (fabs(errOVP)  > 0.02)
        {
            ui->lineEdit_pgs_err->setStyleSheet("background-color: red; color: white");
            ui->lineEdit_pgs_errpercent->setStyleSheet("background-color: red; color: white");
        }
        else
        {
            ui->lineEdit_pgs_err->setStyleSheet("background-color: green; color: white");
            ui->lineEdit_pgs_errpercent->setStyleSheet("background-color: green; color: white");
        }
    }
    else if (ui->tabWidget_nvh->currentIndex() == 2 && currentNVH == CruiseSweep)
    {
        double errSpeed = dataInfo["speedNVHError"];
        double errSpeedP = dataInfo["speedNVHErrorPercent"];
        ui->lineEdit_cs_err->setText(QString::number(errSpeed, 'f', 2));
        ui->lineEdit_cs_errpercent->setText(errSpeedP == -340.25 ? "---" : QString::number(errSpeedP * 100.0, 'f', 2));

        if (fabs(errSpeedP)  > 0.02)
        {
            ui->lineEdit_cs_err->setStyleSheet("background-color: red; color: white");
            ui->lineEdit_cs_errpercent->setStyleSheet("background-color: red; color: white");
        }
        else
        {
            ui->lineEdit_cs_err->setStyleSheet("background-color: green; color: white");
            ui->lineEdit_cs_errpercent->setStyleSheet("background-color: green; color: white");
        }
    }
    else if (ui->tabWidget_nvh->currentIndex() == 1 && currentNVH == DriveAwayAccelerationSweepCurveTrace)
    {
        double errSpeed = dataInfo["speedNVHError"];
        double errSpeedP = dataInfo["speedNVHErrorPercent"];
        ui->lineEdit_dasct_err->setText(QString::number(errSpeed, 'f', 2));
        ui->lineEdit_dasct_errpercent->setText(errSpeedP == -340.25 ? "---" : QString::number(errSpeedP * 100.0, 'f', 2));

        if (fabs(errSpeedP)  > 0.02)
        {
            ui->lineEdit_dasct_err->setStyleSheet("background-color: red; color: white");
            ui->lineEdit_dasct_errpercent->setStyleSheet("background-color: red; color: white");
        }
        else
        {
            ui->lineEdit_dasct_err->setStyleSheet("background-color: green; color: white");
            ui->lineEdit_dasct_errpercent->setStyleSheet("background-color: green; color: white");
        }
    }
}

void PedalRobotUI::ReInitCurvePlot(numNVH nvhNum, int detailNum)
{
    ifPlotEnable = false;

    switch (nvhNum) {
    case DriveAwayAccelerationSweepFixedPedal:
    {
        speedTimeActual.clear(); speedTimeActual.reserve((int)(pdNVH->NVH_DASFP_MODERATETIME * 20.0));
        speedActual.clear(); speedActual.reserve((int)(pdNVH->NVH_DASFP_MODERATETIME * 20.0));

        speedTarget.clear(); speedTarget.reserve(2);
        speedTimeTarget.clear(); speedTimeTarget.reserve(2);
        speedLowerBound.clear(); speedLowerBound.reserve(2);
        speedUpperBound.clear(); speedUpperBound.reserve(2);
        if (detailNum == 0)
        {
            speedTimeTarget.push_back(pdNVH->NVH_DASFP_VERYLOWTIME);
            speedTarget.push_back(0.98 * pdNVH->NVH_DASFP_VERYLOWSPEED);
            speedTimeTarget.push_back(pdNVH->NVH_DASFP_VERYLOWTIME);
            speedTarget.push_back(1.02 * pdNVH->NVH_DASFP_VERYLOWSPEED);
            speedLowerBound.push_back(0.98 * pdNVH->NVH_DASFP_VERYLOWSPEED);
            speedLowerBound.push_back(0.98 * pdNVH->NVH_DASFP_VERYLOWSPEED);
            speedUpperBound.push_back(1.02 * pdNVH->NVH_DASFP_VERYLOWSPEED);
            speedUpperBound.push_back(1.02 * pdNVH->NVH_DASFP_VERYLOWSPEED);
        }
        else if (detailNum == 1)
        {
            speedTimeTarget.push_back(pdNVH->NVH_DASFP_LOWTIME);
            speedTarget.push_back(0.98 * pdNVH->NVH_DASFP_LOWSPEED);
            speedTimeTarget.push_back(pdNVH->NVH_DASFP_LOWTIME);
            speedTarget.push_back(1.02 * pdNVH->NVH_DASFP_LOWSPEED);
            speedLowerBound.push_back(0.98 * pdNVH->NVH_DASFP_LOWSPEED);
            speedLowerBound.push_back(0.98 * pdNVH->NVH_DASFP_LOWSPEED);
            speedUpperBound.push_back(1.02 * pdNVH->NVH_DASFP_LOWSPEED);
            speedUpperBound.push_back(1.02 * pdNVH->NVH_DASFP_LOWSPEED);
        }
        else
        {
            speedTimeTarget.push_back(pdNVH->NVH_DASFP_MODERATETIME);
            speedTarget.push_back(0.98 * pdNVH->NVH_DASFP_MODERATESPEED);
            speedTimeTarget.push_back(pdNVH->NVH_DASFP_MODERATETIME);
            speedTarget.push_back(1.02 * pdNVH->NVH_DASFP_MODERATESPEED);
            speedLowerBound.push_back(0.98 * pdNVH->NVH_DASFP_MODERATESPEED);
            speedLowerBound.push_back(0.98 * pdNVH->NVH_DASFP_MODERATESPEED);
            speedUpperBound.push_back(1.02 * pdNVH->NVH_DASFP_MODERATESPEED);
            speedUpperBound.push_back(1.02 * pdNVH->NVH_DASFP_MODERATESPEED);
        }

        openValueActual.clear(); openValueActual.reserve((int)(pdNVH->NVH_DASFP_MODERATETIME * 20.0));
        openValueTimeActual.clear(); openValueTimeActual.reserve((int)(pdNVH->NVH_DASFP_MODERATETIME * 20.0));

        QVector<double> speedBoundTime;
        speedBoundTime.push_back(speedTimeTarget[0] - 0.2);
        speedBoundTime.push_back(speedTimeTarget[1] + 0.2);
        ui->curvePlot_speed->graph(TargetCurveLayout)->setData(speedTimeTarget, speedTarget);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setData(speedBoundTime, speedUpperBound);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setData(speedBoundTime, speedLowerBound);

        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    case DriveAwayAccelerationSweepCurveTrace:
    {
        speedTimeActual.clear(); speedTimeActual.reserve(2000);
        speedActual.clear(); speedActual.reserve(2000);


        curveTemplate curvePairs;
        if (detailNum == 0)
        {
            curvePairs = pdNVH->NVH_DASCT_VERYLOWCURVE;
        }
        else if (detailNum == 1)
        {
            curvePairs = pdNVH->NVH_DASCT_LOWCURVE;
        }
        else
        {
            curvePairs = pdNVH->NVH_DASCT_MODERATECURVE;
        }

        int len = curvePairs.size();
        speedTarget.clear(); speedTarget.reserve(len);
        speedTimeTarget.clear(); speedTimeTarget.reserve(len);
        speedLowerBound.clear(); speedLowerBound.reserve(len);
        speedUpperBound.clear(); speedUpperBound.reserve(len);

        for (int i = 0; i < len; i++)
        {
            speedTarget.push_back(curvePairs[i].second);
            speedTimeTarget.push_back(curvePairs[i].first);
            speedLowerBound.push_back(0.98 * speedTarget.constLast());
            speedUpperBound.push_back(1.02 * speedTarget.constLast());
        }

        openValueActual.clear(); openValueActual.reserve(2000);
        openValueTimeActual.clear(); openValueTimeActual.reserve(2000);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setData(speedTimeTarget, speedTarget);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setData(speedTimeTarget, speedUpperBound);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setData(speedTimeTarget, speedLowerBound);

        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    case CruiseSweep:
    {
        speedTimeActual.clear(); speedTimeActual.reserve(5000);
        speedActual.clear(); speedActual.reserve(5000);

        speedTarget.clear(); speedTarget.reserve(2);
        speedTimeTarget.clear(); speedTimeTarget.reserve(2);
        speedLowerBound.clear(); speedLowerBound.reserve(2);
        speedUpperBound.clear(); speedUpperBound.reserve(2);

        double beginSpeed = carSpeedShow;
        speedTarget.push_back(beginSpeed);
        speedTarget.push_back(pdNVH->NVH_CS_TERMINATEDSPEED);
        speedTimeTarget.push_back(0.0);
        double terminatedTime = (pdNVH->NVH_CS_TERMINATEDSPEED - beginSpeed) / pdNVH->NVH_CS_ACCELERATION;
        speedTimeTarget.push_back(terminatedTime);
        speedLowerBound.push_back(0.98 * beginSpeed);
        speedLowerBound.push_back(0.98 * pdNVH->NVH_CS_TERMINATEDSPEED);
        speedUpperBound.push_back(1.02 * beginSpeed);
        speedUpperBound.push_back(1.02 * pdNVH->NVH_CS_TERMINATEDSPEED);

        openValueActual.clear(); openValueActual.reserve(5000);
        openValueTimeActual.clear(); openValueTimeActual.reserve(5000);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setData(speedTimeTarget, speedTarget);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setData(speedTimeTarget, speedUpperBound);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setData(speedTimeTarget, speedLowerBound);

        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    case HighLowGearSweep:
    {
        speedTimeActual.clear(); speedTimeActual.reserve(2000);
        speedActual.clear(); speedActual.reserve(2000);

        openValueActual.clear(); openValueActual.reserve(2000);
        openValueTimeActual.clear(); openValueTimeActual.reserve(2000);

        double lowAimOV = ui->lineEdit_hlgs_beginov->text().trimmed().toDouble();
        double upAimOV = ui->lineEdit_hlgs_stopov->text().trimmed().toDouble();
        double usingTime = ui->lineEdit_hlgs_time->text().trimmed().toDouble();

        openValueTimeTarget.clear(); openValueTimeTarget.reserve(2);
        openValueTimeTarget.push_back(0.0); openValueTimeTarget.push_back(usingTime);
        openValueTarget.clear(); openValueTarget.reserve(2);
        openValueTarget.push_back(lowAimOV); openValueTarget.push_back(upAimOV);

        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);

        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setData(openValueTimeTarget, openValueTarget);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    case PassingGearSweep:
    {
        speedTimeActual.clear(); speedTimeActual.reserve(40);
        speedActual.clear(); speedActual.reserve(40);

        openValueActual.clear(); openValueActual.reserve(40);
        openValueTimeActual.clear(); openValueTimeActual.reserve(40);

        double aimOV = ui->comboBox_pgs_aimov->currentText().trimmed().toDouble();

        openValueTimeTarget.clear(); openValueTimeTarget.reserve(2);
        openValueTimeTarget.push_back(0.0); openValueTimeTarget.push_back(2.0);
        openValueTarget.clear(); openValueTarget.reserve(2);
        openValueTarget.push_back(aimOV); openValueTarget.push_back(aimOV);
        openValueLowerBound.clear(); openValueLowerBound.reserve(2);
        openValueLowerBound.push_back(1.02 * aimOV); openValueLowerBound.push_back(1.02 * aimOV);
        openValueUpperBound.clear(); openValueUpperBound.reserve(2);
        openValueUpperBound.push_back(0.98 * aimOV); openValueUpperBound.push_back(0.98 * aimOV);

        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);

        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setData(openValueTimeTarget, openValueTarget);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setData(openValueTimeTarget, openValueUpperBound);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setData(openValueTimeTarget, openValueLowerBound);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(true);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    case AcceleratorPedalSweep:
    {
        speedTimeActual.clear(); speedTimeActual.reserve(2000);
        speedActual.clear(); speedActual.reserve(2000);

        openValueActual.clear(); openValueActual.reserve(2000);
        openValueTimeActual.clear(); openValueTimeActual.reserve(2000);

        double lowAimOV = ui->lineEdit_aps_minov->text().trimmed().toDouble();
        double upAimOV = ui->lineEdit_aps_maxov->text().trimmed().toDouble();
        double usingTime = ui->lineEdit_aps_time->text().trimmed().toDouble();

        openValueTimeTarget.clear(); openValueTimeTarget.reserve(2);
        openValueTimeTarget.push_back(0.0); openValueTimeTarget.push_back(usingTime);
        openValueTarget.clear(); openValueTarget.reserve(2);
        openValueTarget.push_back(lowAimOV); openValueTarget.push_back(upAimOV);

        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);

        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setData(openValueTimeTarget, openValueTarget);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    case General:
    {
        speedTimeActual.clear(); speedTimeActual.reserve(generalArrayLength);
        speedActual.clear(); speedActual.reserve(generalArrayLength);

        openValueActual.clear(); openValueActual.reserve(generalArrayLength);
        openValueTimeActual.clear(); openValueTimeActual.reserve(generalArrayLength);

        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);

        ui->curvePlot_speed->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_speed->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_speed->graph(LowerCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(TargetCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setVisible(true);
        ui->curvePlot_ov->graph(UpperCurveLayout)->setVisible(false);
        ui->curvePlot_ov->graph(LowerCurveLayout)->setVisible(false);

        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
        break;
    }
    default:
        break;
    }
    firstToGeneral = true;
    currentNVH = nvhNum;

    ifPlotEnable = true;
}

void PedalRobotUI::UpdateCurvePlot(const paraMap &dataInfo)
{
    if (!ifPlotEnable)
    {
        return;
    }

    switch (currentNVH)
    {
    case General:
    {
        if (firstToGeneral)
        {
            firstToGeneralTime = dataInfo["prgTime"];
            firstToGeneral = false;
        }
        if (speedTimeActual.count() >= generalArrayLength)
        {
            speedTimeActual.removeAt(0);
            speedActual.removeAt(0);
            openValueTimeActual.removeAt(0);
            openValueActual.removeAt(0);
        }
        speedTimeActual.push_back(dataInfo["prgTime"] - firstToGeneralTime);
        speedActual.push_back(dataInfo["pulseSpeed"]);
        openValueTimeActual.push_back(dataInfo["prgTime"] - firstToGeneralTime);
        openValueActual.push_back(dataInfo["accOV"]);

        ui->curvePlot_speed->graph(ActionCurveLayout)->setData(speedTimeActual, speedActual);
        ui->curvePlot_ov->graph(ActionCurveLayout)->setData(openValueTimeActual, openValueActual);
        double timeCenter = GetShowTimeCenter(dataInfo["prgTime"] - firstToGeneralTime);
        ui->curvePlot_speed->xAxis->setRange(timeCenter - maxShowTime / 2.0, timeCenter + maxShowTime / 2.0);
        ui->curvePlot_ov->xAxis->setRange(timeCenter - maxShowTime / 2.0, timeCenter + maxShowTime / 2.0);
        break;
    }
    case DriveAwayAccelerationSweepFixedPedal:
    case DriveAwayAccelerationSweepCurveTrace:
    case CruiseSweep:
    {
        if (dataInfo["haveRecordNVH"] > 0)
        {
            int length = ui->curvePlot_speed->graph(ActionCurveLayout)->dataCount();
            double lastKey = ui->curvePlot_speed->graph(ActionCurveLayout)->dataMainKey(length - 1);
            if (lastKey != dataInfo["timeNVH"] || (lastKey == dataInfo["timeNVH"] && lastKey == 0.0))
            {
                ui->curvePlot_speed->graph(ActionCurveLayout)->addData(dataInfo["timeNVH"], dataInfo["speedNVH"]);
                ui->curvePlot_ov->graph(ActionCurveLayout)->addData(dataInfo["timeNVH"], dataInfo["accOVNVH"]);

                double timeCenter = GetShowTimeCenter(dataInfo["timeNVH"]);
                ui->curvePlot_speed->xAxis->setRange(timeCenter - maxShowTime / 2.0, timeCenter + maxShowTime / 2.0);
                ui->curvePlot_ov->xAxis->setRange(timeCenter - maxShowTime / 2.0, timeCenter + maxShowTime / 2.0);
            }
        }
        break;
    }

    case HighLowGearSweep:
    case PassingGearSweep:
    case AcceleratorPedalSweep:
    {
        if (dataInfo["haveRecordNVH"] > 0)
        {
            int length = ui->curvePlot_ov->graph(ActionCurveLayout)->dataCount();
            double lastKey = ui->curvePlot_ov->graph(ActionCurveLayout)->dataMainKey(length - 1);
            if (lastKey != dataInfo["timeNVH"] || (lastKey == dataInfo["timeNVH"] && lastKey == 0.0))
            {
                ui->curvePlot_speed->graph(ActionCurveLayout)->addData(dataInfo["timeNVH"], dataInfo["speedNVH"]);
                ui->curvePlot_ov->graph(ActionCurveLayout)->addData(dataInfo["timeNVH"], dataInfo["accOVNVH"]);

                double timeCenter = GetShowTimeCenter(dataInfo["timeNVH"]);
                ui->curvePlot_speed->xAxis->setRange(timeCenter - maxShowTime / 2.0, timeCenter + maxShowTime / 2.0);
                ui->curvePlot_ov->xAxis->setRange(timeCenter - maxShowTime / 2.0, timeCenter + maxShowTime / 2.0);
            }
        }
        break;
    }





    default:
        break;
    }

    plotCount++;
    if (plotCount >=plotMultiplier)
    {
        plotCount = 0;
        ui->curvePlot_speed->replot();
        ui->curvePlot_ov->replot();
    }
}

double PedalRobotUI::GetShowTimeCenter(double currentTime)
{
    return maxShowTime / 2.0 + ((currentTime < maxShowTime * 0.66) ? 0.0 : (currentTime - maxShowTime * 0.66));
}
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------ 内部槽函数 ------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
/*------------------------------------- --------- -------------------------------------*/
void PedalRobotUI::Slot_WidgetShowInformMessageBox(QString str)
{
    MyMessageBox::ShowInformMessageBox(str.toStdString());
}

void PedalRobotUI::Slot_SingleAxisPressed()
{
    int axis=0;
    int direction=0;
    if(sender() == ui->pushButton_plus1){
        axis=0; direction=1;
    }else if(sender() == ui->pushButton_plus2){
        axis=1; direction=1;
    }else if(sender() == ui->pushButton_minus1){
        axis=0; direction=-1;
    }else if(sender() == ui->pushButton_minus2){
        axis=1; direction=-1;
    }

    emit Signal_SingleAxisMove(axis, direction);
}

void PedalRobotUI::Slot_SingleAxisReleased()
{
     emit Signal_EnterIdle();
}

void PedalRobotUI::Slot_NVH_DASFP_RadioBtnClicked(bool checked)
{
    if (!checked) return;

    int level = 0;
    if (sender() == ui->radioButton_dasfp_vl) level = 0;
    else if (sender() == ui->radioButton_dasfp_l) level = 1;
    else if (sender() == ui->radioButton_dasfp_m) level = 2;

    emit Signal_NVH_DASFP_GetRecommandOV(level);
}

void PedalRobotUI::Slot_NVH_HLGS_SlowRunPressed()
{
    bool ifPos = false;
    if(sender() == ui->pushButton_hlgs_slowplus) ifPos = true;
    else if(sender() == ui->pushButton_hlgs_slowdecline) ifPos = false;

    emit Signal_NVH_HLGS_SingleAxisSlowMove(ifPos);
}

void PedalRobotUI::Slot_NVH_HLGS_SlowRunReleased()
{
    emit Signal_EnterIdle();
}

void PedalRobotUI::Slot_UpdateWidget(paraMap mapContent, int mapLength)
{
    if (mapContent.count() != mapLength)
    {
        return;
    }

    ui->lineEdit_d1->setText(QString::number(mapContent["brkPos"], 'f', 2));
    ui->lineEdit_d2->setText(QString::number(mapContent["accPos"], 'f', 2));
    ui->progressBar_brake_ov->setValue(qRound(mapContent["brkOV"]));
    ui->progressBar_accelerator_ov->setValue(qRound(mapContent["accOV"]));
    ui->lineEdit_CanBrakeOpenValue->setText(QString::number(mapContent["brkOV"], 'f', 1));
    ui->lineEdit_CanAccOpenValue->setText(QString::number(mapContent["accOV"], 'f', 1));
    ui->lineEdit_CanSpeed->setText(QString::number(mapContent["canSpeed"], 'f', 2));
    ui->lineEdit_adVoltage->setText(QString::number(mapContent["adVoltage"], 'f', 1));
    ui->lineEdit_pulseFrequency->setText(QString::number(mapContent["pulseFrequency"], 'f', 0));
    ui->lineEdit_UsbMP412Speed->setText(QString::number(mapContent["pulseSpeed"], 'f', 2));
    ui->lineEdit_CanPowerMode->setText(QString::number(mapContent["powerMode"], 'f', 0));
    ui->lineEdit_brk_maxov->setText(QString::number(mapContent["brkOVUB"], 'f', 0));
    ui->lineEdit_brk_minov->setText(QString::number(mapContent["brkOVLB"], 'f', 0));
    ui->lineEdit_acc_maxov->setText(QString::number(mapContent["accOVUB"], 'f', 0));
    ui->lineEdit_acc_minov->setText(QString::number(mapContent["accOVLB"], 'f', 0));
    accMaxOV = mapContent["accOVUB"];
    accMinOV = mapContent["accOVLB"];
    carSpeedShow = mapContent["pulseSpeed"];

    ui->tabWidget_nvh->setTabText(6, QString::fromStdString("车速: ") +
                                  QString::number(mapContent["pulseSpeed"], 'f', 2) +
            QString::fromStdString("km/h 开度: ") +
            QString::number(mapContent["accOV"], 'f', 1) +
            QString::fromStdString("%"));

    const int powerMode = (int)mapContent["powerMode"];
    QString powerModeStyleSheet;
    switch(powerMode){
        case ePowerModeType::Run:
        powerModeStyleSheet = "background-color:green; color: white";
        break;
    case ePowerModeType::Off:
        powerModeStyleSheet = "background-color:red; color: white";
        break;
    default:
        powerModeStyleSheet = "background-color:blue; color: white";
        break;
    }
    ui->lineEdit_powerOnOff->setStyleSheet(powerModeStyleSheet);

    if ((int)mapContent["goHome"] > -1)
    {
        EnableButtonsForGoHome((int)mapContent["goHome"] > 0);
    }

    UpdateCurvePlot(mapContent);
    UpdateNVHWidget(mapContent);
}

void PedalRobotUI::Slot_UpdateWidgetForPreTestFinished()
{
    ui->pushButton_pretest->setText(tr(" 预测试 "));
    MyMessageBox::ShowInformMessageBox("预测试结束!");

    // 改变UI
    ui->tabWidget_nvh->setEnabled(true);

    // 打开绘图
    ReInitCurvePlot(General);
}

void PedalRobotUI::Slot_UpdateWidgetForO2VWaitBegin(bool ifStartWait)
{
    ui->pushButton_pretest->setText(tr(" 续/停 "));
    if (ifStartWait)
    {
        MyMessageBox::ShowInformMessageBox("请切换到D档并维持车辆怠速后，点击按钮继续预测试!");
    }
    else
    {
        MyMessageBox::ShowInformMessageBox("请通过转毂将速度下降到怠速后，点击按钮继续预测试!");
    }
}

void PedalRobotUI::Slot_NVH_DASFP_UpdateWidgetForRecommandOV(double recommandedOV)
{
    ui->lineEdit_dasfp_aimov->setText(QString::number(recommandedOV, 'f', 1));
}

void PedalRobotUI::Slot_NVH_DASFP_Finished(int status, double error, double errorp)
{
    if (status == 0)
    {
        ReInitCurvePlot(General);

        ui->pushButton_dasfp_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);
        EnableButtonsForSingle();

        ui->lineEdit_dasfp_err->setText("0.00");
        ui->lineEdit_dasfp_errpercent->setText("0.00");
        ui->lineEdit_dasfp_err->setStyleSheet("background-color: green; color: white");
        ui->lineEdit_dasfp_errpercent->setStyleSheet("background-color: green; color: white");
    }
    else
    {
        ui->pushButton_dasfp_ovctrl->setText(tr(" 确认结束 "));

        ui->lineEdit_dasfp_err->setText(QString::number(error, 'f', 2));
        ui->lineEdit_dasfp_errpercent->setText(QString::number(100.0 * errorp, 'f', 2));

        if (fabs(errorp) > 0.02)
        {
            ui->lineEdit_dasfp_err->setStyleSheet("background-color: red; color: white");
            ui->lineEdit_dasfp_errpercent->setStyleSheet("background-color: red; color: white");
        }
        else
        {
            ui->lineEdit_dasfp_err->setStyleSheet("background-color: green; color: white");
            ui->lineEdit_dasfp_errpercent->setStyleSheet("background-color: green; color: white");
        }
    }
}

void PedalRobotUI::Slot_NVH_DASCT_Finished(int status)
{
    if (status == 0)
    {
        ReInitCurvePlot(General);

        ui->pushButton_dasct_ctctrl->setText(tr("开始运行"));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);

        EnableButtonsForSingle();
    }
    else
    {
        ui->pushButton_dasct_ctctrl->setText(tr(" 确认结束 "));
    }
}

void PedalRobotUI::Slot_NVH_CS_TestOVFinished()
{
    ui->pushButton_cs_ovctrl->setText(tr(" 测试开度 "));
    ui->tab_dasfp->setEnabled(true);
    ui->tab_dasct->setEnabled(true);
    ui->tab_hlgs->setEnabled(true);
    ui->tab_pgs->setEnabled(true);
    ui->tab_aps->setEnabled(true);
    ui->pushButton_cs_atctrl->setEnabled(true);
    EnableButtonsForSingle();
}

void PedalRobotUI::Slot_NVH_CS_Finished(int status)
{
    if (status == 0)
    {
        ReInitCurvePlot(General);

        ui->pushButton_cs_atctrl->setText(tr("开始运行"));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);

        ui->pushButton_cs_ovctrl->setEnabled(true);
        EnableButtonsForSingle();
    }
    else
    {
        ui->pushButton_cs_atctrl->setText(tr(" 确认结束 "));
    }
}

void PedalRobotUI::Slot_NVH_HLGS_KeepSpeedFinished()
{
    ui->pushButton_hlgs_speed->setText(tr(" 保持车速 "));
    ui->tab_dasfp->setEnabled(true);
    ui->tab_dasct->setEnabled(true);
    ui->tab_cs->setEnabled(true);
    ui->tab_pgs->setEnabled(true);
    ui->tab_aps->setEnabled(true);
    ui->pushButton_hlgs_slowplus->setEnabled(true);
    ui->pushButton_hlgs_slowdecline->setEnabled(true);
    ui->pushButton_hlgs_ovctrl->setEnabled(true);
    EnableButtonsForSingle();
}

void PedalRobotUI::Slot_NVH_HLGS_Finished(int status)
{
    if (status == 0)
    {
        ReInitCurvePlot(General);

        ui->pushButton_hlgs_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);

        ui->pushButton_hlgs_slowplus->setEnabled(true);
        ui->pushButton_hlgs_slowdecline->setEnabled(true);
        ui->pushButton_hlgs_speed->setEnabled(true);
        EnableButtonsForSingle();
    }
    else
    {
        ui->pushButton_hlgs_ovctrl->setText(tr(" 确认结束 "));
    }
}

void PedalRobotUI::Slot_NVH_PGS_Finished(int status)
{
    if (status == 0)
    {
        ReInitCurvePlot(General);

        ui->pushButton_pgs_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);
        EnableButtonsForSingle();

        ui->lineEdit_pgs_err->setText("0.00");
        ui->lineEdit_pgs_errpercent->setText("0.00");
        ui->lineEdit_pgs_err->setStyleSheet("background-color: green; color: white");
        ui->lineEdit_pgs_errpercent->setStyleSheet("background-color: green; color: white");
    }
    else
    {
        ui->pushButton_pgs_ovctrl->setText(tr(" 确认结束 "));
    }
}

void PedalRobotUI::Slot_NVH_APS_Finished(int status)
{
    if (status == 0)
    {
        ReInitCurvePlot(General);

        ui->pushButton_aps_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        EnableButtonsForSingle();
    }
    else
    {
        ui->pushButton_aps_ovctrl->setText(tr(" 确认结束 "));
    }
}

void PedalRobotUI::on_pushButton_origin_clicked()
{
    bool flag = MyMessageBox::ShowQuestionMessageBox("返回原点时请勿进行其他操作!\n开始回原?");
    if(!flag)
    {
        PRINTF(LOG_DEBUG, "%s is canceled\n",__func__);
        return;
    }
    PRINTF(LOG_DEBUG, "%s starts\n", __func__);

    emit  Signal_GoHome();
}

void PedalRobotUI::on_pushButton_softStop_clicked()
{
    emit Signal_SoftStop();

    // UI修改
    ui->tabWidget_nvh->setEnabled(true);

    // 打开绘图
    ReInitCurvePlot(General);
}

void PedalRobotUI::on_pushButton_softStop_liftPedals_clicked()
{
    emit Signal_SoftStopAndLift();

    // UI修改
    ui->tabWidget_nvh->setEnabled(true);

    // 打开绘图
    ReInitCurvePlot(General);
}

void PedalRobotUI::on_pushButton_pretest_clicked()
{
    if (pdNVH->GetPreTestStatus())
    {
        if (pdNVH->GetO2VTestStatus())
        {
            int getReq = MyMessageBox::ShowQueryMessageBox("继续预测试或者停止预测试?", "继续", "停止", "取消");
            switch (getReq) {
            case 0:
                // 继续预测试
                emit Signal_ContinueO2VTest();

                // 改变UI
                ui->pushButton_pretest->setText(tr(" 停止 "));
                break;
            case 1:
            {
                // 停止测试
                emit Signal_StopPreTestRightNow();

                // 改变UI
                ui->pushButton_pretest->setText(tr(" 预测试 "));
                ui->tabWidget_nvh->setEnabled(true);

                // 打开绘图
                ReInitCurvePlot(General);
                break;
            }
            case 2:
                break;
            default:
                break;
            }
        }
        else
        {
            // 停止测试
            emit Signal_StopPreTestRightNow();

            // 改变UI
            ui->pushButton_pretest->setText(tr(" 预测试 "));
            ui->tabWidget_nvh->setEnabled(true);

            // 打开绘图
            ReInitCurvePlot(General);
        }
    }
    else
    {
        int getReq = MyMessageBox::ShowQueryMessageBox("是否进行预测试或者读取已有的测试文件?", "测试", "读取", "取消");
        switch (getReq) {
        case 0:
        {
            // 关闭绘图
            ifPlotEnable = false;

            // 提醒预测试挡位条件
            MyMessageBox::ShowInformMessageBox("为了进行预测试，请先把挡位切换到N挡!");

            double positionInterval_t = 2.0;
            double backDistance_t = 6.0;
            bool ifAutoBrk_t = true;
            double NotAutoBrkLimit_t = 0.0;
            bool ifAutoAcc_t = true;
            double NotAutoAccLimit_t = 0.0;
            bool ifO2V_t = Configuration::Instance()->sysControlParams[0] < 0.5 ? true : false;

            int o2VNum_t = 10;
            double o2VStopTime_t = 30.0;
            double o2VStopVelocity_t = 140.0;
            bool ifDecelerateByBrk_t = true;

            bool ifTransfer_t = true;
            int wantedAxisNum_t = 0;
            bool ifBothAxis_t = true;
            int delayCircuit_t = 6;

#ifdef ENABLE_BRAKE_NVH
            wantedAxisNum_t = 0;
            ifBothAxis_t = true;
#else
            wantedAxisNum_t = 1;
            ifBothAxis_t = false;
#endif

#ifdef ENABLE_BRAKE_NVH
            bool getRes1 = MyMessageBox::ShowQuestionMessageBox("是否自动寻找刹车下极限?");
            if (getRes1) ifAutoBrk_t = true;
            else
            {
                ifAutoBrk_t = false;
                bool ok;
                double tempDouble = QInputDialog::getDouble(NULL, tr("Input"), tr("请输入刹车下极限时的刹车开度:"), 0.0, 0.0, 100.0, 1, &ok);
                if (ok) NotAutoBrkLimit_t = tempDouble;
                else return;
            }
#endif

            bool getRes2 = MyMessageBox::ShowQuestionMessageBox("是否自动寻找油门下极限?");
            if (getRes2) ifAutoAcc_t = true;
            else
            {
                ifAutoAcc_t = false;
                bool ok;
                double tempDouble = QInputDialog::getDouble(NULL, tr("Input"), tr("请输入油门下极限时的油门开度:"), 0.0, 0.0, 100.0, 1, &ok);
                if (ok) NotAutoAccLimit_t = tempDouble;
                else return;
            }

            // 进行预测试
            emit Signal_BeginPreTest(positionInterval_t, backDistance_t, ifAutoBrk_t, NotAutoBrkLimit_t,
                                     ifAutoAcc_t, NotAutoAccLimit_t, ifO2V_t, o2VNum_t, o2VStopTime_t, o2VStopVelocity_t,
                                     ifDecelerateByBrk_t, ifTransfer_t, wantedAxisNum_t, ifBothAxis_t, delayCircuit_t);

            // 改变UI
            ui->pushButton_pretest->setText(tr(" 停止 "));
            ui->tabWidget_nvh->setEnabled(false);
            break;
        }
        case 1:
        {
            bool ifStraightRead = MyMessageBox::ShowQuestionMessageBox("是否直接读取当前测试文件?");
            QString filePath;

            if (!ifStraightRead)
            {
                filePath = QFileDialog::getOpenFileName(NULL, tr("选择测试文件"), QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + pdNVH->GetP2OTransferDirectory()));
                if(filePath.isEmpty()){
                    return;
                }
            }

            // 读取预测试文件
            emit Signal_ReadPreTestFile(filePath, ifStraightRead);
            break;
        }
        case 2:
            break;
        default:
            break;
        }
    }
}

void PedalRobotUI::on_pushButton_dasfp_ovctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else if (pdNVH->GetNVHStatus() == 0)
    {
        int detailMode = 0;
        if (ui->radioButton_dasfp_l->isChecked()) detailMode = 1;
        else if (ui->radioButton_dasfp_m->isChecked()) detailMode = 2;
        else detailMode = 0;
        double aimOV = ui->lineEdit_dasfp_aimov->text().trimmed().toDouble();

        ReInitCurvePlot(DriveAwayAccelerationSweepFixedPedal, detailMode);

        emit Signal_NVH_DASFP_Start(aimOV, detailMode);

        ui->pushButton_dasfp_ovctrl->setText(tr(" 停止运行 "));
        ui->tab_dasct->setEnabled(false);
        ui->tab_cs->setEnabled(false);
        ui->tab_hlgs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);
        EnableButtonsForSingle(false);
    }
    else
    {
        ReInitCurvePlot(General);

        emit Signal_NVH_ConfirmStop();

        ui->pushButton_dasfp_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);
        EnableButtonsForSingle();

        ui->lineEdit_dasfp_err->setText("0.00");
        ui->lineEdit_dasfp_errpercent->setText("0.00");
        ui->lineEdit_dasfp_err->setStyleSheet("background-color: green; color: white");
        ui->lineEdit_dasfp_errpercent->setStyleSheet("background-color: green; color: white");
    }
}

void PedalRobotUI::on_pushButton_dasct_ctctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else if (pdNVH->GetNVHStatus() == 0)
    {
        if (carSpeedShow > 10.0)
        {
            MyMessageBox::ShowInformMessageBox("当前车速大于10km/h，请等怠速后重试!");
            return;
        }

        double accGain = Configuration::Instance()->sysControlParams[4];
        double rTime = Configuration::Instance()->sysControlParams[5];

        int detailMode = 0;
        if (ui->radioButton_dasct_l->isChecked()) detailMode = 1;
        else if (ui->radioButton_dasct_m->isChecked()) detailMode = 2;
        else detailMode = 0;

        ReInitCurvePlot(DriveAwayAccelerationSweepCurveTrace, detailMode);

        emit Signal_NVH_DASCT_Start(accGain, rTime, detailMode);

        ui->pushButton_dasct_ctctrl->setText(tr(" 停止运行 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_cs->setEnabled(false);
        ui->tab_hlgs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);

        EnableButtonsForSingle(false);
    }
    else
    {
        ReInitCurvePlot(General);

        emit Signal_NVH_ConfirmStop();

        ui->pushButton_dasct_ctctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);

        EnableButtonsForSingle();
    }
}

void PedalRobotUI::on_pushButton_cs_ovctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else
    {
        double aimOV = ui->lineEdit_cs_initialov->text().trimmed().toDouble();
        emit Signal_NVH_CS_TestOV(aimOV);

        ui->pushButton_cs_ovctrl->setText(tr(" 停止测试 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_dasct->setEnabled(false);
        ui->tab_hlgs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);

        ui->pushButton_cs_atctrl->setEnabled(false);
        EnableButtonsForSingle(false);
    }
}

void PedalRobotUI::on_pushButton_cs_atctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else if (pdNVH->GetNVHStatus() == 0)
    {
        double accGain = Configuration::Instance()->sysControlParamsWltc[0];
        double rTime = Configuration::Instance()->sysControlParamsWltc[1];
        ReInitCurvePlot(CruiseSweep);

        emit Signal_NVH_CS_Start(accGain, rTime);

        ui->pushButton_cs_atctrl->setText(tr(" 停止运行 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_dasct->setEnabled(false);
        ui->tab_hlgs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);

        ui->pushButton_cs_ovctrl->setEnabled(false);
        EnableButtonsForSingle(false);
    }
    else
    {
        ReInitCurvePlot(General);

        emit Signal_NVH_ConfirmStop();

        ui->pushButton_cs_atctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);

        ui->pushButton_cs_ovctrl->setEnabled(true);
        EnableButtonsForSingle();
    }
}

void PedalRobotUI::on_pushButton_hlgs_speed_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else
    {
        double aimSpeed = ui->lineEdit_hlgs_speed->text().trimmed().toDouble();
        double aimAcceleration = Configuration::Instance()->sysControlParamsWltc[2];
        double approachTime = Configuration::Instance()->sysControlParamsWltc[3];
        double accelerationGain = Configuration::Instance()->sysControlParamsWltc[4];

        emit Signal_NVH_HLGS_KeepSpeed(aimSpeed, aimAcceleration,
                                       approachTime, accelerationGain);

        ui->pushButton_hlgs_speed->setText(tr(" 停止保持 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_dasct->setEnabled(false);
        ui->tab_cs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);
        ui->pushButton_hlgs_slowplus->setEnabled(false);
        ui->pushButton_hlgs_slowdecline->setEnabled(false);
        ui->pushButton_hlgs_ovctrl->setEnabled(false);
        EnableButtonsForSingle(false);
    }
}

void PedalRobotUI::on_pushButton_hlgs_ovctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else if (pdNVH->GetNVHStatus() == 0)
    {
        double lowAimOV = ui->lineEdit_hlgs_beginov->text().trimmed().toDouble();
        double upAimOV = ui->lineEdit_hlgs_stopov->text().trimmed().toDouble();
        double usingTime = ui->lineEdit_hlgs_time->text().trimmed().toDouble();
        double advancedOV = Configuration::Instance()->sysControlParamsWltc[5];
        ReInitCurvePlot(HighLowGearSweep);

        emit Signal_NVH_HLGS_Start(lowAimOV, upAimOV, usingTime, advancedOV);

        ui->pushButton_hlgs_ovctrl->setText(tr(" 停止运行 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_dasct->setEnabled(false);
        ui->tab_cs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);

        ui->pushButton_hlgs_slowplus->setEnabled(false);
        ui->pushButton_hlgs_slowdecline->setEnabled(false);
        ui->pushButton_hlgs_speed->setEnabled(false);
        EnableButtonsForSingle(false);
    }
    else
    {
        ReInitCurvePlot(General);

        emit Signal_NVH_ConfirmStop();

        ui->pushButton_hlgs_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);

        ui->pushButton_hlgs_slowplus->setEnabled(true);
        ui->pushButton_hlgs_slowdecline->setEnabled(true);
        ui->pushButton_hlgs_speed->setEnabled(true);
        EnableButtonsForSingle();
    }
}

void PedalRobotUI::on_pushButton_pgs_ovctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else if (pdNVH->GetNVHStatus() == 0)
    {
        double aimOV = ui->comboBox_pgs_aimov->currentText().trimmed().toDouble();

        ReInitCurvePlot(PassingGearSweep);

        emit Signal_NVH_PGS_Start(aimOV);

        ui->pushButton_pgs_ovctrl->setText(tr(" 停止运行 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_dasct->setEnabled(false);
        ui->tab_cs->setEnabled(false);
        ui->tab_hlgs->setEnabled(false);
        ui->tab_aps->setEnabled(false);
        EnableButtonsForSingle(false);
    }
    else
    {
        ReInitCurvePlot(General);

        emit Signal_NVH_ConfirmStop();

        ui->pushButton_pgs_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_aps->setEnabled(true);
        EnableButtonsForSingle();

        ui->lineEdit_pgs_err->setText("0.00");
        ui->lineEdit_pgs_errpercent->setText("0.00");
        ui->lineEdit_pgs_err->setStyleSheet("background-color: green; color: white");
        ui->lineEdit_pgs_errpercent->setStyleSheet("background-color: green; color: white");
    }
}

void PedalRobotUI::on_comboBox_pgs_aimov_activated(int index)
{
    QString str;
    switch (index) {
    case 0:
        str = QString::number((accMaxOV - accMinOV) * 0.35 + accMinOV, 'f', 1);
        break;
    case 1:
        str = QString::number((accMaxOV - accMinOV) * 0.75 + accMinOV, 'f', 1);
        break;
    case 2:
        str = QString::number(accMaxOV, 'f', 1);
        break;
    default:
        break;
    }
    ui->comboBox_pgs_aimov->setCurrentText(str);
}

void PedalRobotUI::on_pushButton_aps_maxroute_clicked()
{
    ui->lineEdit_aps_maxov->setText(QString::number(accMaxOV, 'f', 1));
    ui->lineEdit_aps_minov->setText(QString::number(accMinOV, 'f', 1));
}

void PedalRobotUI::on_pushButton_aps_ovctrl_clicked()
{
    if (pdNVH->GetNVHStatus() == 1)
    {
        emit Signal_NVH_StopNow();
    }
    else if (pdNVH->GetNVHStatus() == 0)
    {
        double lowAimOV = ui->lineEdit_aps_minov->text().trimmed().toDouble();
        double upAimOV = ui->lineEdit_aps_maxov->text().trimmed().toDouble();
        double usingTime = ui->lineEdit_aps_time->text().trimmed().toDouble();
        double advancedOV = Configuration::Instance()->sysControlParamsWltc[6];
        ReInitCurvePlot(AcceleratorPedalSweep);

        emit Signal_NVH_APS_Start(lowAimOV, upAimOV, usingTime, advancedOV);

        ui->pushButton_aps_ovctrl->setText(tr(" 停止运行 "));
        ui->tab_dasfp->setEnabled(false);
        ui->tab_dasct->setEnabled(false);
        ui->tab_cs->setEnabled(false);
        ui->tab_hlgs->setEnabled(false);
        ui->tab_pgs->setEnabled(false);
        EnableButtonsForSingle(false);
    }
    else
    {
        ReInitCurvePlot(General);

        emit Signal_NVH_ConfirmStop();

        ui->pushButton_aps_ovctrl->setText(tr(" 开始运行 "));
        ui->tab_dasfp->setEnabled(true);
        ui->tab_dasct->setEnabled(true);
        ui->tab_cs->setEnabled(true);
        ui->tab_hlgs->setEnabled(true);
        ui->tab_pgs->setEnabled(true);
        EnableButtonsForSingle();
    }
}
