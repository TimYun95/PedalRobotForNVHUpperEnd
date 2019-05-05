#ifndef PEDALROBOTNVH_H
#define PEDALROBOTNVH_H
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include <QObject>
#include <QMap>
#include <QDateTime>
#include <QTime>

#include "uri.h"
#include "util/highprecisiontimer.h"

#include "configuration.h"

#include "robottype/pedalrobot/pedalrobotusbprocessor.h"
#include "robottype/pedalrobot/pedalpretest.h"

class PedalRobotNVH : public QObject
{
    Q_OBJECT
public: // 基本函数
    explicit PedalRobotNVH(QObject *parent = 0);
    ~PedalRobotNVH();

    typedef QMap<QString, double> paraMap;
    typedef std::vector< std::pair<double, double> > curveTemplate;

public: // 对外函数
    URI* GetURI(); // 获得基础URI
    void StopAllModuleMotion(); // 停止所有可能在运动的模块
    void StartInternalTimer(); // 开始内部定时器
    void StopInternalTimer(); // 关闭内部定时器
    bool GetPreTestStatus(); // 获得预测试状态
    std::string GetP2OTransferDirectory(); // 获得p2o转存路径
    std::string GetO2VTransferDirectory(); // 获得p2o转存路径
    bool GetO2VTestStatus(); // 获得开度速度测试状态

    int GetNVHStatus(); // 获得NVH运行状态

private: // 内部函数
    void InsertCurveTemplateDASCT(); // 载入DAS-CT工况下的模板曲线
    void InitSignalSlot(); // 初始化信号槽
    void InitParams(); // 初始化参数
    void UpdateLogicControl(); // 更新控制逻辑
    void UpdateLocalLogic(); // 更新本类逻辑

    bool ResetOriginFileByDeathValue(); // 更新回原文件
    bool ResetSoftStopFile(); // 更新停止文件

    paraMap UpdateTransferParams(int* const dataLength); // 更新待发送的界面参数

    void UpdateNVHLogic(); // 更新NVH逻辑
    void UpdateNVHLogic_NVH_DASFP(); // 更新NVH_DASFP逻辑
    void EndNVHLogic_NVH_DASFP(); // 结束NVH_DASFP逻辑
    void GetNVHError_NVH_DASFP(double* const err, double* const errp); // 获得NVH_DASFP误差

    void UpdateNVHLogic_NVH_DASCT(); // 更新NVH_DASCT逻辑
    void EndNVHLogic_NVH_DASCT(); // 结束NVH_DASCT测试逻辑


    void UpdateNVHLogic_NVH_CS_TestOV(); // 更新NVH_CS测试开度逻辑
    void EndNVHLogic_NVH_CS_TestOV(); // 结束NVH_CS测试开度逻辑
    void UpdateNVHLogic_NVH_CS(); // 更新NVH_CS逻辑
    void EndNVHLogic_NVH_CS(); // 结束NVH_CS逻辑
    void UpdateNVHLogic_NVH_HLGS_KeepSpeed(); // 更新NVH_HLGS保持车速逻辑
    void EndNVHLogic_NVH_HLGS_KeepSpeed(); // 结束NVH_HLGS保持车速逻辑
    void UpdateNVHLogic_NVH_HLGS(); // 更新NVH_HLGS逻辑
    void EndNVHLogic_NVH_HLGS(); // 结束NVH_HLGS逻辑
    void UpdateNVHLogic_NVH_PGS(); // 更新NVH_PGS逻辑
    void EndNVHLogic_NVH_PGS(); // 结束NVH_PGS逻辑
    void UpdateNVHLogic_NVH_APS(); // 更新NVH_APS逻辑
    void EndNVHLogic_NVH_APS(); // 结束NVH_APS逻辑

    void SaveNVHRecordDatas(QString workMode, QString workParams = ""); // 保存NVH记录数据
    void ImmediateEndNVHCtrl(bool ifLiftPedal = true); // 立即结束NVH逻辑
    void NotImmediateEndNVHCtrl(); // 非立即结束的NVH逻辑

    void SendPedalCmd(double accPos); // 发送油门踏板绝对位置指令
    void SendServoCmd(double accPos); // 发送油门踏板相对位置指令

    void InitialSpeedAndAcceleration(double initialSpeed = 0.0, double initialAcceleration = 0.0); // 初始化速度和加速度记录以及其误差记录
    void GetSpeedAndAcceleration(double refSpeed, double refAcceleration); // 获得当前的速度和加速度反馈并计算误差

    void RefreshControlMode(); // 更新控制方式

signals: // 信号
    void Signal_UpdateWidget(paraMap mapContent, int mapLength); // 更新界面信息
    void Signal_UpdateWidgetForPreTestFinished(); // 更新预测试结束界面
    void Signal_UpdateWidgetForO2VWaitBegin(bool ifStartWait); // 更新开度速度关系测量等待开始界面

    void Signal_NVH_DASFP_UpdateWidgetForRecommandOV(double recommandedOV); // 更新NVH-DASFP工况下的推荐开度界面
    void Signal_NVH_DASFP_Finished(int status, double error = 0.0, double errorp = 0.0); // NVH-DASFP工况结束
    void Signal_NVH_DASCT_Finished(int status); // NVH-DASCT工况结束
    void Signal_NVH_CS_TestOVFinished(); // NVH-CS工况测试开度结束
    void Signal_NVH_CS_Finished(int status); // NVH-CS工况结束
    void Signal_NVH_HLGS_KeepSpeedFinished(); // NVH-HLGS工况保持车速结束
    void Signal_NVH_HLGS_Finished(int status); // NVH-HLGS工况结束
    void Signal_NVH_PGS_Finished(int status); // NVH-PGS工况结束
    void Signal_NVH_APS_Finished(int status); // NVH-APS工况结束

private slots: // 内部槽函数
    void Slot_SingleAxisMove(int axisNum, int direction); // 单轴运动
    void Slot_EnterIdle(); // 进入空闲模式
    void Slot_GoHome(); // 回原
    void Slot_SoftStop(); // 停止
    void Slot_SoftStopAndLift(); // 停止回抬
    void Slot_BeginPreTest(double positionInterval, double backDistance, bool ifAutoBrk, double NotAutoBrkLimit, bool ifAutoAcc, double NotAutoAccLimit, bool ifO2V, int o2VNum, double o2VStopTime, double o2VStopVelocity, bool ifDecelerateByBrk, bool ifTransfer, int wantedAxisNum, bool ifBothAxis, int delayCircuit); // 开始预测试
    void Slot_StopPreTestRightNow(); // 立刻停止预测试
    void Slot_ReadPreTestFile(QString pickFilePath, bool ifStraightRead); // 读取预测试文件
    void Slot_ContinueO2VTest(); // 继续开度速度测试

    void Slot_LogicUpdateTimeOut(int currentTimeoutCount, int intervalMultiplier); // 逻辑处理定时器触发
    void Slot_WidgetUpdateTimeOut(); // 界面信息处理定时器触发

    void Slot_PreTestFinished(); // 预测试结束
    void Slot_O2VWaitBegin(bool ifStartWait); // 开度速度关系测量等待开始

    void Slot_NVH_DASFP_GetRecommandOV(int level); // NVH-DASFP工况下获得推荐开度
    void Slot_NVH_DASFP_Start(double aimOV, int level); // NVH-DASFP工况开始

    void Slot_NVH_DASCT_Start(double accGain, double rectifyTime, int level); // NVH-DASCT工况开始

    void Slot_NVH_CS_TestOV(double aimOV); // NVH-CS工况测试开度
    void Slot_NVH_CS_Start(double accGain, double rectifyTime); // NVH-CS工况开始

    void Slot_NVH_HLGS_KeepSpeed(double aimSpeed, double aimAcceleration,
                                 double approachTime, double accelerationGain); // NVH_HLGS工况保持车速
    void Slot_NVH_HLGS_SingleAxisSlowMove(bool ifPos); // NVH_HLGS工况缓踩/松油门
    void Slot_NVH_HLGS_Start(double lowAimOV, double upAimOV, double usingTime, double advancedPos); // NVH_HLGS工况开始

    void Slot_NVH_PGS_Start(double aimOV); // NVH_PGS工况开始

    void Slot_NVH_APS_Start(double lowAimOV, double upAimOV, double usingTime, double advancedPos); // NVH_APS工况开始

    void Slot_NVH_ConfirmStop(); // 确认NVH结束
    void Slot_NVH_StopNow(); // NVH工况停止

public: // 对外变量
    const std::string recordNVHDataDirectory; // NVH数据记录路径

    const double NVH_DASFP_VERYLOWTIME; // NVH-DASFP中VL工况定时
    const double NVH_DASFP_VERYLOWSPEED; // NVH-DASFP中VL工况定速
    const double NVH_DASFP_LOWTIME; // NVH-DASFP中L工况定时
    const double NVH_DASFP_LOWSPEED; // NVH-DASFP中L工况定速
    const double NVH_DASFP_MODERATETIME; // NVH-DASFP中M工况定时
    const double NVH_DASFP_MODERATESPEED; // NVH-DASFP中M工况定速

    const double NVH_DASCT_VERYLOWTIME; // NVH-DASCT中VL工况定时
    const double NVH_DASCT_VERYLOWMAXSPEED; // NVH-DASCT中VL工况最大速度
    const double NVH_DASCT_LOWTIME; // NVH-DASCT中L工况定时
    const double NVH_DASCT_LOWMAXSPEED; // NVH-DASCT中L工况最大速度
    const double NVH_DASCT_MODERATETIME; // NVH-DASCT中M工况定时
    const double NVH_DASCT_MODERATEMAXSPEED; // NVH-DASCT中M工况最大速度
    curveTemplate NVH_DASCT_VERYLOWCURVE; // NVH-DASCT中VL工况曲线
    curveTemplate NVH_DASCT_LOWCURVE; // NVH-DASCT中L工况曲线
    curveTemplate NVH_DASCT_MODERATECURVE; // NVH-DASCT中M工况曲线

    const double NVH_CS_ACCELERATION; // NVH-CS工况的既定加速度
    const double NVH_CS_TERMINATEDSPEED; // NVH-CS工况的终止速度

private: // 内部变量
    HighPrecisionTimer LogicUpdateTimer; // 逻辑更新定时器
    HighPrecisionTimer WidgetUpdateTimer; // 界面更新定时器

    URI* pUri; // 基础URI
    PedalRobotUSBProcessor* pdUSB; // USB数据更新类
    PedalPreTest* pdPreTest; // 预测试类

    int goHomeState; // 更新回原状态

    qint64 programmaStartTime; // 程序开始时刻
    double programmaStartTimeInterval; // 程序开始时间

    int statusNVHRun; // NVH工况是否在运行
    double startNVHTime; // NVH工况开始时刻
    double startNVHTimeInterval; // NVH工况开始时间
    double intervalNVHTimeInterval; // NVH工况间隔时间
    double startNVHAccOV; // NVH工况开始开度
    int caseNVH; // NVH工况步骤
    const int caseChangeStopInterval; // NVH工况步骤转变间隔
    int caseChangeStopIntervalCount; // NVH工况步骤转变间隔计数
    std::vector< std::vector<double> > recordDatasNVH; // NVH工况数据记录

    const double dvAlpha; // 加速度指数滤波系数
    double speed[4], dv[3]; // 速度以及加速度记录
    double error[3], dvError[3]; // 速度以及加速度误差

    bool ifOpenLoop; // 是否闭环控制
    double pedalGain_NVH_Loop; // NVH闭环踏板增益
    double threshold_NVH_Loop; // NVH闭环切换阈值
    const double openLoopTime; // 开环时间

    bool ifRun_NVH_DASFP; // NVH-DASFP工况是否在运行
    double aimOV_NVH_DASFP; // NVH-DASFP工况的目标开度
    double stopTime_NVH_DASFP; // NVH-DASFP工况的终止时刻

    bool ifRun_NVH_DASCT; // NVH-DASCT工况是否在运行
    double accelerationGain_NVH_DASCT; // NVH-DASCT工况加速度增益
    double rectifyTime_NVH_DASCT; // NVH-DASCT工况矫正时间
    double deltaSpeed_NVH_DASCT; // NVH-DASCT工况最大加速度对应速度差
    double stopTime_NVH_DASCT; // NVH-DASCT工况的终止时刻
    curveTemplate curveTemplate_NVH_DASCT; // NVH-DASCT工况的模板曲线

    bool ifRun_NVH_CS_TestOV; // NVH-CS工况测试开度是否在运行
    double aimOV_NVH_CS; // NVH-CS工况的目标开度
    bool ifRun_NVH_CS; // NVH-CS工况是否在运行
    double initialSpeed_NVH_CS; // NVH-CS工况初始速度
    double accelerationGain_NVH_CS; // NVH-CS工况加速度增益
    double rectifyTime_NVH_CS; // NVH-CS工况矫正时间
    double deltaSpeed_NVH_CS; // NVH-CS工况最大加速度对应速度差

    bool ifRun_NVH_HLGS_KeepSpeed; // NVH-HLGS工况保持车速是否在运行
    double aimSpeed_NVH_HLGS; // NVH-HLGS工况目标车速
    bool accelerated_NVH_HLGS; // NVH-HLGS工况是否提速
    double acceleration_NVH_HLGS; // NVH-HLGS工况提速加速度
    double approachTime_NVH_HLGS_AccelerationChanging; // NVH-HLGS工况变加速度段预计时间
    double speedDifference_NVH_HLGS_AccelerationChanging; // NVH-HLGS工况变加速度段速度差范围
    double accelerationGain_NVH_HLGS; // NVH-HLGS工况加速度增益
    bool ifRun_NVH_HLGS; // NVH-HLGS工况是否在运行
    double aimLowOV_NVH_HLGS; // NVH-HLGS工况的下目标开度
    double aimUpOV_NVH_HLGS; // NVH-HLGS工况的上目标开度
    double usingTime_NVH_HLGS; // NVH-HLGS工况持续的时间
    double advancedPos_NVH_HLGS; // NVH-HLGS工况位置提前量

    bool ifRun_NVH_PGS; // NVH-PGS工况是否在运行
    double aimOV_NVH_PGS; // NVH-PGS工况的目标开度

    bool ifRun_NVH_APS; // NVH-APS工况是否在运行
    double aimLowOV_NVH_APS; // NVH-APS工况的下目标开度
    double aimUpOV_NVH_APS; // NVH-APS工况的上目标开度
    double usingTime_NVH_APS; // NVH-APS工况持续的时间
    double advancedPos_NVH_APS; // NVH-APS工况位置提前量

    const double positionError; // 位置分辨率
    const double openValueError; // 开度分辨率
    const double overTimeInterval; // 超时时间
    const double leastTimeInterval; // 踩下至少需要时间
    const double waitTimeInterval; // 等待时间

};

#endif // PEDALROBOTNVH_H
