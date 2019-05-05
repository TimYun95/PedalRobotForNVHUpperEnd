#ifndef PEDALROBOTUI_H
#define PEDALROBOTUI_H

#include <QWidget>
#include <QMap>

#include "uri.h"

#include "mywidget/qcustomplot.h"
#include "configuration.h"

#include "robottype/pedalrobot/pedalrobotnvh.h"

namespace Ui {
class PedalRobotUI;
}

class PedalRobotUI : public QWidget
{
    Q_OBJECT

public: // 基本函数
    explicit PedalRobotUI(QWidget *parent = 0);
    ~PedalRobotUI();

    typedef QMap<QString, double> paraMap;
    typedef std::vector< std::pair<double, double> > curveTemplate;

    enum ePowerModeType
    {
        Off=0,
        Accessory,
        Run,
        CrankRequest,

        MaxPowerModeType
    };

    enum eCustomPlotWidgetGraphLayout{
        TargetCurveLayout,//示教文件的曲线
        ActionCurveLayout,//动作曲线
        UpperCurveLayout,//速度上界
        LowerCurveLayout,//速度下界
    };

    enum numNVH
    {
        DriveAwayAccelerationSweepFixedPedal = 0,
        DriveAwayAccelerationSweepCurveTrace,
        CruiseSweep,
        HighLowGearSweep,
        PassingGearSweep,
        AcceleratorPedalSweep,

        General = 99
    };


public: // 对外函数

private: // 内部函数
    void InitWidgets(); // 初始化界面
    void InitCurvePlot(); // 初始化曲线绘制
    void InitSignalSlot(); // 初始化信号槽连接
    void EndThread(); // 正常结束线程

    void EnableButtonsForGoHome(bool enable); // 回原状态和按钮状态使能
    void EnableButtonsForSingle(bool enable = true); // 单轴按钮使能
    void UpdateNVHWidget(const paraMap& dataInfo); // 更新NVH面板中的参数

    void ReInitCurvePlot(numNVH nvhNum, int detailNum = 0); // 重绘曲线图
    void UpdateCurvePlot(const paraMap& dataInfo); // 更新曲线图
    double GetShowTimeCenter(double currentTime); // 获得曲线图中央时刻

signals: // 信号
    void Signal_SingleAxisMove(int axisNum, int direction); // 单轴运动
    void Signal_EnterIdle(); // 进入空闲模式
    void Signal_GoHome(); // 回原
    void Signal_SoftStop(); // 停止
    void Signal_SoftStopAndLift(); // 停止回抬
    void Signal_BeginPreTest(
            double positionInterval,
            double backDistance,
            bool ifAutoBrk,
            double NotAutoBrkLimit,
            bool ifAutoAcc,
            double NotAutoAccLimit,
            bool ifO2V,
            int o2VNum,
            double o2VStopTime,
            double o2VStopVelocity,
            bool ifDecelerateByBrk,
            bool ifTransfer,
            int wantedAxisNum,
            bool ifBothAxis,
            int delayCircuit); // 开始预测试
    void Signal_StopPreTestRightNow(); // 立刻停止预测试
    void Signal_ReadPreTestFile(QString pickFilePath, bool ifStraightRead); // 读取预测试文件
    void Signal_ContinueO2VTest(); // 继续开度速度测试

    void Signal_NVH_DASFP_GetRecommandOV(int level); // NVH-DASFP工况下获得推荐开度
    void Signal_NVH_DASFP_Start(double aimOV, int level); // NVH-DASFP工况开始

    void Signal_NVH_DASCT_Start(double accGain, double rectifyTime, int level); // NVH-DASCT工况开始

    void Signal_NVH_CS_TestOV(double aimOV); // NVH-CS工况测试开度
    void Signal_NVH_CS_Start(double accGain, double rectifyTime); // NVH-CS工况开始

    void Signal_NVH_HLGS_KeepSpeed(double aimSpeed, double aimAcceleration,
                                   double approachTime, double accelerationGain); // NVH_HLGS工况保持车速
    void Signal_NVH_HLGS_SingleAxisSlowMove(bool ifPos); // NVH_HLGS工况缓踩/松油门
    void Signal_NVH_HLGS_Start(double lowAimOV, double upAimOV, double usingTime, double advancedOV); // NVH_HLGS工况开始

    void Signal_NVH_PGS_Start(double aimOV); // NVH_PGS工况开始

    void Signal_NVH_APS_Start(double lowAimOV, double upAimOV, double usingTime, double advancedPos); // NVH_APS工况开始

    void Signal_NVH_ConfirmStop(); // 确认NVH结束
    void Signal_NVH_StopNow(); // NVH工况停止

private slots: // 内部槽函数
    void Slot_WidgetShowInformMessageBox(QString str); // 显示弹框

    void Slot_SingleAxisPressed(); // 单轴按钮按下
    void Slot_SingleAxisReleased(); // 单轴按钮松开
    void Slot_NVH_DASFP_RadioBtnClicked(bool checked); // NVH-DASFP工况下单选框按下
    void Slot_NVH_HLGS_SlowRunPressed(); // NVH-HLGS工况缓踩/松按钮按下
    void Slot_NVH_HLGS_SlowRunReleased(); // NVH-HLGS工况缓踩/松按钮松开

    void Slot_UpdateWidget(paraMap mapContent, int mapLength); // 更新界面参数信息
    void Slot_UpdateWidgetForPreTestFinished(); // 更新预测试结束界面
    void Slot_UpdateWidgetForO2VWaitBegin(bool ifStartWait); // 更新开度速度关系测量等待开始界面

    void Slot_NVH_DASFP_UpdateWidgetForRecommandOV(double recommandedOV); // 更新NVH-DASFP工况下的推荐开度界面
    void Slot_NVH_DASFP_Finished(int status, double error, double errorp); // NVH-DASFP工况结束
    void Slot_NVH_DASCT_Finished(int status); // NVH-DASCT工况结束
    void Slot_NVH_CS_TestOVFinished(); // NVH-CS工况测试开度结束
    void Slot_NVH_CS_Finished(int status); // NVH-CS工况结束
    void Slot_NVH_HLGS_KeepSpeedFinished(); // NVH-HLGS工况保持车速结束
    void Slot_NVH_HLGS_Finished(int status); //NVH-HLGS工况结束
    void Slot_NVH_PGS_Finished(int status); // NVH-PGS工况结束
    void Slot_NVH_APS_Finished(int status); // NVH-APS工况结束

    void on_pushButton_origin_clicked(); // 回原
    void on_pushButton_softStop_clicked(); // 停止
    void on_pushButton_softStop_liftPedals_clicked(); // 停止回抬
    void on_pushButton_pretest_clicked(); // 预测试

    void on_pushButton_dasfp_ovctrl_clicked(); // DAS-FP开始

    void on_pushButton_dasct_ctctrl_clicked(); // DAS-CT开始

    void on_pushButton_cs_ovctrl_clicked(); // CS测试开度
    void on_pushButton_cs_atctrl_clicked(); // CS开始
    void on_pushButton_hlgs_speed_clicked(); // HLGS保持车速
    void on_pushButton_hlgs_ovctrl_clicked(); // HLGS开始
    void on_pushButton_pgs_ovctrl_clicked(); // PGS开始
    void on_comboBox_pgs_aimov_activated(int index); // PGS选择目标开度
    void on_pushButton_aps_maxroute_clicked(); // APS最大开度范围
    void on_pushButton_aps_ovctrl_clicked(); // APS开始


public: // 对外变量

private: // 内部变量
    Ui::PedalRobotUI *ui;

    URI *pUri; // 基础URI
    QThread pdThread; // NVH控制线程
    PedalRobotNVH* pdNVH; // NVH控制类

    bool ifPlotEnable; // 是否允许绘制曲线
    numNVH currentNVH; // 当前NVH工况号，用以区分绘图
    bool firstToGeneral; // 首次进入普通模式，重新计算初始时间
    double firstToGeneralTime; // 首次进入普通模式的时刻

    QVector<double> speedTimeActual; // 速度曲线实际时间
    QVector<double> speedActual; // 速度曲线实际速度

    QVector<double> speedTimeTarget; // 速度曲线模板时间
    QVector<double> speedLowerBound; // 速度曲线下限
    QVector<double> speedUpperBound; // 速度曲线上限
    QVector<double> speedTarget; // 速度曲线目标速度

    QVector<double> openValueTimeActual; // 开度曲线实际时间
    QVector<double> openValueActual; // 开度曲线实际速度

    QVector<double> openValueTimeTarget; // 开度曲线模板时间
    QVector<double> openValueLowerBound; // 开度曲线下限
    QVector<double> openValueUpperBound; // 开度曲线上限
    QVector<double> openValueTarget; // 开度曲线目标速度

    const int generalArrayLength; // 普通模式下的绘图数组最大长度
    const double maxShowTime; // 最大显示时间跨度
    const double maxShowSpeed; // 最大显示速度
    const double maxShowOpenValue; // 最大显示开度
    const int plotMultiplier; // 曲线重绘分时
    int plotCount; // 曲线重绘计数

    double accMaxOV; // 油门最大开度
    double accMinOV; // 油门最小开度

    double carSpeedShow; // 用于显示的车速

};

#endif // PEDALROBOTUI_H
