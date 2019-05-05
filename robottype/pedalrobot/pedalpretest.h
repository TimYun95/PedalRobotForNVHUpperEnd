#ifndef PEDALPRETEST_H
#define PEDALPRETEST_H

#include <QObject>

#include "uri.h"
#include "configuration.h"

#include "fileoperation/normalfile.h"
#include "assistantfunc/fileassistantfunc.h"
#include "common/printf.h"

#include "robottype/pedalrobot/pedalrobotusbprocessor.h"

class PedalPreTest : public QObject
{
    Q_OBJECT
public:
    explicit PedalPreTest(PedalRobotUSBProcessor* pdRobot, QObject* parent = 0);
    ~PedalPreTest();

public:
    bool ifPreTestRun; // 预测试标志位
    const std::string preTestFileName; // 预测试文件名称
    const std::string posToOVFileName; // 位置开度文件名称
    const std::string transferDirectoryP2O; // 位置开度默认转存位置
    const std::string ovToVelocityFileName; // 开度速度文件名称
    const std::string transferDirectoryO2V; // 开度速度默认转存位置
    bool o2VTestPauseForSignal; // 开度速度关系测试单程切换信号等待
    bool ifEnableSingleMoveDuringPreTest; // 允许在预测试中使用单轴

signals:
    void Signal_PreTestFinished(); // 预测试结束
    void Signal_O2VWaitBegin(bool ifStartWait); // 开度速度关系测量等待开始

private:
    PedalRobotUSBProcessor *pdRobot;

    std::vector< std::pair<double, double> > preTestDatas; // 预测试数据记录
    std::vector< std::pair<double, double> > brkPositionToOpenValuePositive; // 刹车正向位置到开度关系
    std::vector< std::pair<double, double> > accPositionToOpenValuePositive; // 油门正向位置到开度关系
    std::vector< std::pair<double, double> > brkPositionToOpenValueNegative; // 刹车负向位置到开度关系
    std::vector< std::pair<double, double> > accPositionToOpenValueNegative; // 油门负向位置到开度关系

    double preTestAimMotorPosition; // 预测试目标踏板位置
    int preTestAxisNum; // 预测试踏板是否为油门轴
    bool ifPreTestAxisBoth; // 是否预测试两个踏板

    int preTestCount; // 预测试周期计数器
    int preTestDelayCount; // 预测试延迟周期
    double preTestPositionToOpenValueInterval; // 预测试踏板位置和开度关系时的位置间隔

    double preTestDeathOpenValue; // 预测试踏板死区开度
    double preTestInitialPedalPosition; // 预测试初始测试点
    double preTestInitialPedalBackDistance; // 预测试初始测试点计算回退位置距离
    bool ifAutoTransfer; // 是否自动转存文件

    std::vector< std::vector< std::pair<double, double> > > openValueToVelocity; // 开度速度关系
    std::vector< std::pair<double, double> > o2VRoundData; // 开度速度关系单程数据

    bool ifTestO2V; // 是否测试开度速度关系
    int o2VPartNum; // 开度速度关系分段个数
    double o2VAimOV; // 开度速度关系测试目标开度
    double o2VRoundTime; // 开度速度关系测试单程最大时间
    double o2VMaxVelocity; // 开度速度关系测试单程最大车速
    bool o2VBrkUsedToDecelerate; // 开度速度关系测试使用刹车踏板来减速

    bool ifAutoBrkLimitNeg; // 是否自动测试刹车下极限
    double manualBrkLimitNegOV; // 手动输入刹车下极限对应的开度
    bool ifAutoAccLimitNeg; // 是否自动测试油门下极限
    double manualAccLimitNegOV; // 手动输入油门下极限对应的开度

    QString transferFileHead; // 转存文件名称头暂存

    const double baseOpenValue; // 基础油门开度
    const double positionError; // 位置分辨率

    qint64 o2VStartTime; // 开度速度关系测试开始时刻
    double o2VStartTimeInterval; // 开度速度关系测试开始时间

    /*
     * -1 -->  Inital Conf
     * 0   -->  Not Ready
     * 1   -->  To Be Ready
     * 2   -->  Get Ready
     * 3   -->  One Axis Slow Forward To Find Critical Position
     * 4   -->  Critical Position Found And Go To Initial Position
     * 5   -->  Initial Position Arrived And Prepare To Test Axis Open Value
     * 6   -->  Record Initial Axis Open Value And Go To Positive Test
     * 7   -->  Do This Round Open Value Test
     * 8   -->  This Round Test Is Finished
     * 9   -->  Judge If Positive Test Finished
     * 10 -->  Record Initial Axis Open Value And Go To Negative Test
     * 11 -->  Do This Round Open Value Test
     * 12 -->  This Round Test Is Finished
     * 13 -->  Judge If Negative Test Finished And If Whole Open Value Test Finished
     * 14 -->  After Position To Open Value Test Wait For Shift To Begin Open Value To Velocity Test
     * 15 -->  Shift Is Ready
     *
     * 30 -->  Begin Open Value To Velocity Test And Go Back Lift Position First
     * 31 -->  Arrive At Lift Position
     * 32 -->  Enter Listening Mode Of ARM
     * 33 -->  Send Need Position For Specific Open Value
     * 34 -->  Arrive At Need Position
     * 35 -->  Reach Signal Round Stop Condition
     * 36 -->  Acc Pedal Back
     * 37 -->  Leave Listening Mode Of ARM
     * 38 -->  Wait For Velocity To Come Down By Brk (To 39) Or Drum (To 43) And Save Datas
     * 39 -->  Brk Down To BrkAfterGoHome
     * 40 -->  If Speed Lower Than 20 KMH Than Brk Down To Max
     * 41 -->  If Speed Lower Than 2 KMH Than Brk Up
     * 42 -->  Arrive At Brk Up
     * 43 -->  Wait For Signal To Ensure Speed To Near Zero
     * 44 -->  Prepare For Next Open Value To Speed Test
     * 45 -->  Save Datas
     *
     *
     * 99 --> PreTest To End
     */
    int preTestStatus; // 预测试状态

public:
    void BeginPreTest(double positionInterval = 2.0,
                      double backDistance = 6.0,
                      bool ifAutoBrk = true,
                      double NotAutoBrkLimit = 0.0,
                      bool ifAutoAcc = true,
                      double NotAutoAccLimit = 0.0,
                      bool ifO2V = true,
                      int o2VNum = 10,
                      double o2VStopTime = 30.0,
                      double o2VStopVelocity = 140.0,
                      bool ifDecelerateByBrk = true,
                      bool ifTransfer = true,
                      int wantedAxisNum = 1,
                      bool ifBothAxis = false,
                      int delayCircuit = 6); // 开始预测试
    void UpdatePreTestLogic(); // 更新预测试逻辑
    void ImmediateEndPreTest(bool ifLiftPedal = true); // 立即结束测试

    void ReadSavedPreTestFile(); // 读取已经保存的位置开度文件
    double GetPositionFromOpenValue(double currentOpenValue, double aimOpenValue, bool ifAccAxis = true); // 获得开度对应的踏板位置
    double GetLimitOpenValue(int axisNum, bool ifMax); // 获得单轴的极限开度值

    void TransferFileToAnotherPath(QString filePath); // 转存文件
    void PickTransferFileFromAnotherPath(QString filePath); // 取回转存文件

    double GetOVFromTimeAndSpeed(double time, double speed); // 从时间和速度获得踏板位置

private:
    void InitialPreTest(); // 初始化预测试参数
    void EndPreTest(); // 结束预测试

    bool RefreshPreTestTxt(bool ifForceToBeZero = false); // 刷新预测试txt文件
    void GetAccPosToOV(bool ifPositive = true); // 获得油门位置到开度关系
    void GetBrkPosToOV(bool ifPositive = true); // 获得刹车位置到开度关系
    void SaveConfParams(bool ifStart = false); // 保存部分配置参数
    void SavePosToOVToTxt(); // 保存位置开度关系到txt文件
    void SendOVCPedalCmd(double value, bool ifAbs, int axisNum); // 开度控制踏板指令发送

    void GetOVToSpeed(); // 获得开度到速度关系
    void SaveOVToSpeedToTxt(); // 保存开度速度关系到txt文件

    void TransferP2OToAnotherPath(QString filePath); // 转存P2O文件
    void TransferO2VToAnotherPath(QString filePath); // 转存O2V文件
    void PickTransferP2OFileFromAnotherPath(QString filePath); // 取回转存P2O文件
    void PickTransferO2VFileFromAnotherPath(QString filePath); // 取回转存O2V文件

    double OneDimensionInterpolation(double x1, double y1, double x2, double y2, double x); // 一维插值函数 (x - x1) / (x2 - x1) * (y2 - y1) + y1
};

#endif // PEDALPRETEST_H
