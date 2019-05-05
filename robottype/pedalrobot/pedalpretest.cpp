#include "pedalpretest.h"

#include <QDateTime>
#include <QTime>

#include "util/timeutil.h"

PedalPreTest::PedalPreTest(PedalRobotUSBProcessor *pdRobot, QObject *parent)
    : QObject(parent),
      preTestFileName("preTest.txt"),
      posToOVFileName("p2o.txt"),
      transferDirectoryP2O("/p2o_files/"),
      ovToVelocityFileName("o2v.txt"),
      transferDirectoryO2V("/o2v_files/"),
      baseOpenValue(3.0),
      positionError(0.1)
{
    this->pdRobot = pdRobot;

    InitialPreTest();
}

PedalPreTest::~PedalPreTest()
{

}

void PedalPreTest::InitialPreTest()
{
    preTestDatas.clear();
    preTestDatas.reserve(100);
    brkPositionToOpenValuePositive.clear();
    brkPositionToOpenValuePositive.reserve(100);
    accPositionToOpenValuePositive.clear();
    accPositionToOpenValuePositive.reserve(100);
    brkPositionToOpenValueNegative.clear();
    brkPositionToOpenValueNegative.reserve(100);
    accPositionToOpenValueNegative.clear();
    accPositionToOpenValueNegative.reserve(100);

    preTestDelayCount = 6;
    preTestPositionToOpenValueInterval = 2.0;

    preTestCount = 0;
    preTestAimMotorPosition = 0.0;
    preTestAxisNum = 1;
    ifPreTestAxisBoth = false;

    preTestDeathOpenValue = 0.0;
    preTestInitialPedalPosition = 0.0;
    preTestInitialPedalBackDistance = 4.0;
    ifAutoTransfer = true;

    ifEnableSingleMoveDuringPreTest = false;

    openValueToVelocity.clear();
    openValueToVelocity.reserve(30);
    o2VRoundData.clear();
    o2VRoundData.reserve(600);

    ifTestO2V = true;
    o2VPartNum = 10;
    o2VAimOV = 0.0;
    o2VRoundTime = 0.0;
    o2VMaxVelocity = 0.0;
    o2VBrkUsedToDecelerate = true;
    o2VTestPauseForSignal = false;

    ifAutoBrkLimitNeg = true;
    manualBrkLimitNegOV = 0.0;
    ifAutoAccLimitNeg = true;
    manualAccLimitNegOV = 0.0;

    preTestStatus = -1;
    ifPreTestRun = false;
}

void PedalPreTest::BeginPreTest(double positionInterval,
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
                                int delayCircuit)
{
    preTestDatas.clear();
    preTestDatas.reserve(100);

    preTestAxisNum = wantedAxisNum;
    if (preTestAxisNum > 1)
    {
        EndPreTest();
        return;
    }

    ifPreTestAxisBoth = ifBothAxis;
    if (ifPreTestAxisBoth)
    {
        preTestAxisNum = 0;
    }

    preTestDelayCount = delayCircuit;
    preTestPositionToOpenValueInterval = positionInterval;
    preTestInitialPedalBackDistance = backDistance;

    ifAutoTransfer = ifTransfer;

    ifEnableSingleMoveDuringPreTest = false;

    openValueToVelocity.clear();
    openValueToVelocity.reserve(30);
    o2VRoundData.clear();
    o2VRoundData.reserve(600);

    ifTestO2V = ifO2V;
    o2VPartNum = o2VNum;
    o2VRoundTime = o2VStopTime;
    o2VMaxVelocity = o2VStopVelocity;
    o2VBrkUsedToDecelerate = ifDecelerateByBrk;
    o2VTestPauseForSignal = false;

    ifAutoBrkLimitNeg = ifAutoBrk;
    manualBrkLimitNegOV = NotAutoBrkLimit;
    ifAutoAccLimitNeg = ifAutoAcc;
    manualAccLimitNegOV = NotAutoAccLimit;

    preTestCount = preTestDelayCount;
    preTestAimMotorPosition = 0.0;
    preTestStatus = -1;
    ifPreTestRun = true;

    PRINTF(LOG_DEBUG, "%s: pre-test starts.\n", __func__);
}

void PedalPreTest::UpdatePreTestLogic()
{
    if (!ifPreTestRun)
    {
        return;
    }

    double robotTheta[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        robotTheta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
    }

    if (!ifPreTestAxisBoth) // 单轴测试修正
    {
        robotTheta[0] = 0.0;
    }

    double pedalOpenValue = 0.0;
    if (preTestAxisNum == 0) pedalOpenValue = pdRobot->GetBrakeOV();
    else pedalOpenValue = pdRobot->GetAcceleratorOV();

    switch (preTestStatus) {
    case -1:
    {
        // 更新conf文件
        SaveConfParams(true);

        // 延时设置
        preTestCount = preTestDelayCount;

        // 跳转到0
        preTestStatus = 0;

        PRINTF(LOG_DEBUG, "%s: pre-test status at -1, refresh conf file.\n", __func__);
        break;
    }
    case 0:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 更新文件 回到<min，min>
            if (!RefreshPreTestTxt())
            {
                EndPreTest();
            }

            // 发送action文件
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到1
            preTestStatus = 1;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 1:
    {
        if (((ifAutoBrkLimitNeg && robotTheta[0] < 0.1) || (!ifAutoBrkLimitNeg && fabs(robotTheta[0] - Configuration::Instance()->limitNeg[0]) < 0.1)) &&
                ((ifAutoAccLimitNeg && robotTheta[1] < 0.1) || (!ifAutoAccLimitNeg && fabs(robotTheta[1] - Configuration::Instance()->limitNeg[1]) < 0.1)))
        {
            if (preTestCount <= 0) // 到达目标
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到2
                preTestStatus = 2;
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 2:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 是否需要自动检测下极限点
            if (preTestAxisNum == 0 && !ifAutoBrkLimitNeg)
            {
                // 初始设置
                preTestInitialPedalPosition = robotTheta[preTestAxisNum];

                // 设置目标位置
                preTestAimMotorPosition = preTestInitialPedalPosition + preTestPositionToOpenValueInterval;

                // 记录初值
                preTestDatas.clear();
                preTestDatas.reserve(100);
                preTestDatas.push_back(std::make_pair(robotTheta[preTestAxisNum], manualBrkLimitNegOV));

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到7
                preTestStatus = 7;
                return;
            }
            if (preTestAxisNum == 1 && !ifAutoAccLimitNeg)
            {
                // 初始设置
                preTestInitialPedalPosition = robotTheta[preTestAxisNum];

                // 设置目标位置
                preTestAimMotorPosition = preTestInitialPedalPosition + preTestPositionToOpenValueInterval;

                // 记录初值
                preTestDatas.clear();
                preTestDatas.reserve(100);
                preTestDatas.push_back(std::make_pair(robotTheta[preTestAxisNum], manualAccLimitNegOV));

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到7
                preTestStatus = 7;
                return;
            }

            preTestDeathOpenValue = pedalOpenValue;

            // 更新文件 待测踏板向前
            if (!RefreshPreTestTxt())
            {
                EndPreTest();
            }

            // 发送action文件
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到3
            preTestStatus = 3;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 3:
    {
        if (pedalOpenValue > (preTestDeathOpenValue + 1.0))
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到4
            preTestStatus = 4;
        }
        break;
    }
    case 4:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 计算初始测试点
            preTestInitialPedalPosition = robotTheta[preTestAxisNum] - preTestInitialPedalBackDistance;
            if (preTestInitialPedalPosition < 0.0) preTestInitialPedalPosition=0.0;

            // 更新文件 回到初始测试点
            if (!RefreshPreTestTxt())
            {
                EndPreTest();
            }

            // 发送action文件
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到5
            preTestStatus = 5;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 5:
    {
        if (std::fabs(robotTheta[preTestAxisNum] - preTestInitialPedalPosition) < 0.1)
        {
            if (preTestCount <= 0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 设置目标位置
                preTestAimMotorPosition = preTestInitialPedalPosition;

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到6
                preTestStatus = 6;
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 6:
    {
        if (preTestCount <= 0)
        {
            // 记录初值
            preTestDatas.push_back(std::make_pair(robotTheta[preTestAxisNum], pedalOpenValue));

            // 延时设置
            preTestCount = preTestDelayCount;

            // 设置目标位置
            preTestAimMotorPosition += preTestPositionToOpenValueInterval;

            // 跳转到7
            preTestStatus = 7;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 7:
    {
        // 更新文件
        if (!RefreshPreTestTxt())
        {
            EndPreTest();
        }

        // 发送action文件
        const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
        UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

        // 延时设置
        preTestCount = preTestDelayCount;

        // 跳转到8
        preTestStatus = 8;

        PRINTF(LOG_DEBUG, "%s: pre-test status at 7, '%d' axis is going to %f.\n", __func__, preTestAxisNum, preTestAimMotorPosition);
        break;
    }
    case 8:
    {
        if (fabs(robotTheta[preTestAxisNum] - preTestAimMotorPosition) < 0.1)
        {
            if (preTestCount <= 0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到9
                preTestStatus = 9;

                PRINTF(LOG_DEBUG, "%s: pre-test status at 8, '%d' axis is at %f.\n", __func__, preTestAxisNum, preTestAimMotorPosition);
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 9:
    {
        if (preTestCount <= 0)
        {
            // 记录过程值
            preTestDatas.push_back(std::make_pair(robotTheta[preTestAxisNum], pedalOpenValue));

            // 延时设置
            preTestCount = preTestDelayCount;

            // 判断正向测试是否结束
            if (preTestAimMotorPosition >= std::min(Configuration::Instance()->limitPos[preTestAxisNum], Configuration::Instance()->pedalPositionLimit[preTestAxisNum]))
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 获得踏板开度关系
                if (preTestAxisNum == 0)
                {
                    GetBrkPosToOV();
                }
                else
                {
                    GetAccPosToOV();
                }

                // 延时设置
                preTestCount = preTestDelayCount;

                // 预测试数据还原
                preTestDatas.clear();
                preTestDatas.reserve(100);

                // 跳转到10
                preTestStatus = 10;
            }
            else
            {
                // 设置目标位置
                preTestAimMotorPosition += preTestPositionToOpenValueInterval;

                // 目标限幅
                if (preTestAimMotorPosition > std::min(Configuration::Instance()->limitPos[preTestAxisNum], Configuration::Instance()->pedalPositionLimit[preTestAxisNum]) - positionError)
                {
                    preTestAimMotorPosition = std::min(Configuration::Instance()->limitPos[preTestAxisNum], Configuration::Instance()->pedalPositionLimit[preTestAxisNum]);
                }

                // 跳转到7
                preTestStatus = 7;

                PRINTF(LOG_DEBUG, "%s: pre-test status at 9, continuing to test current pedal.\n", __func__);
            }
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 10:
    {
        if (preTestCount <= 0)
        {
            // 记录初值
            preTestDatas.push_back(std::make_pair(robotTheta[preTestAxisNum], pedalOpenValue));

            // 延时设置
            preTestCount = preTestDelayCount;

            // 设置目标位置
            preTestAimMotorPosition -= preTestPositionToOpenValueInterval;

            // 跳转到11
            preTestStatus = 11;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 11:
    {
        // 更新文件
        if (!RefreshPreTestTxt())
        {
            EndPreTest();
        }

        // 发送action文件
        const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
        UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

        // 延时设置
        preTestCount = preTestDelayCount;

        // 跳转到12
        preTestStatus = 12;

        PRINTF(LOG_DEBUG, "%s: pre-test status at 11, '%d' axis is going to %f.\n", __func__, preTestAxisNum, preTestAimMotorPosition);
        break;
    }
    case 12:
    {
        if (fabs(robotTheta[preTestAxisNum] - preTestAimMotorPosition) < 0.1)
        {
            if (preTestCount <= 0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到13
                preTestStatus = 13;

                PRINTF(LOG_DEBUG, "%s: pre-test status at 12, '%d' axis is at %f.\n", __func__, preTestAxisNum, preTestAimMotorPosition);
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 13:
    {
        if (preTestCount <= 0)
        {
            // 记录过程值
            preTestDatas.push_back(std::make_pair(robotTheta[preTestAxisNum], pedalOpenValue));

            // 延时设置
            preTestCount = preTestDelayCount;

            // 判断负向测试是否结束
            if (preTestAimMotorPosition <= preTestInitialPedalPosition)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 获得踏板开度关系
                if (preTestAxisNum == 0)
                {
                    GetBrkPosToOV(false);
                }
                else
                {
                    GetAccPosToOV(false);
                }

                if (ifPreTestAxisBoth && preTestAxisNum == 0) // 双轴测试且目前在测刹车踏板
                {
                    preTestDatas.clear(); // 预测试数据还原
                    preTestDatas.reserve(100);

                    preTestAimMotorPosition = 0.0; // 目标位置还原
                    preTestAxisNum = 1; // 继续测试油门

                    preTestStatus = 0; // 跳转到0

                    PRINTF(LOG_DEBUG, "%s: pre-test status at 13, continuing to test acc pedal.\n", __func__);
                }
                else if (ifPreTestAxisBoth && preTestAxisNum == 1) // 双轴测试且目前在测油门踏板
                {
                    SavePosToOVToTxt(); // 保存踏板开度关系到txt
                    SaveConfParams(); // 保存部分参数

                    preTestCount = preTestDelayCount; // 延时设置

                    if (ifTestO2V)
                    {
                        preTestStatus = 14; // 跳转到14
                    }
                    else
                    {
                        preTestStatus = 99; // 跳转到99
                    }

                    PRINTF(LOG_DEBUG, "%s: pre-test status at 13, finishing to test brk pedal.\n", __func__);
                }
                else
                {
                    SavePosToOVToTxt(); // 保存踏板开度关系到txt
                    SaveConfParams(); // 保存部分参数

                    preTestCount = preTestDelayCount; // 延时设置
                    if (preTestAxisNum == 1)
                    {
                        if (ifTestO2V)
                        {
                            preTestStatus = 14; // 跳转到14
                        }
                        else
                        {
                            preTestStatus = 99; // 跳转到99
                        }
                    }
                    else
                    {
                        preTestStatus = 99; // 跳转到99
                    }

                    PRINTF(LOG_DEBUG, "%s: pre-test status at 13, finishing to test specific pedal.\n", __func__);
                }
            }
            else
            {
                // 设置目标位置
                preTestAimMotorPosition -= preTestPositionToOpenValueInterval;

                // 目标限幅
                if (preTestAimMotorPosition < preTestInitialPedalPosition + positionError)
                {
                    preTestAimMotorPosition = preTestInitialPedalPosition;
                }

                // 跳转到11
                preTestStatus = 11;

                PRINTF(LOG_DEBUG, "%s: pre-test status at 13, continuing to test current pedal.\n", __func__);
            }
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 14:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 开始等待
            o2VTestPauseForSignal = true;
            ifEnableSingleMoveDuringPreTest = true;
            emit Signal_O2VWaitBegin(true);

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到15
            preTestStatus = 15;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 15:
    {
        if (!o2VTestPauseForSignal)
        {
            if (preTestCount <= 0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();
                ifEnableSingleMoveDuringPreTest = false;

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到30
                preTestStatus = 30;
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }

        /*---------------------------------------------------------------------*/

    case 30:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 更新文件 回到<min，min>
            if (!RefreshPreTestTxt())
            {
                EndPreTest();
            }

            // 发送action文件
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到31
            preTestStatus = 31;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 31:
    {
        if (fabs(robotTheta[0] - Configuration::Instance()->limitNeg[0] - 2.0) < 0.1 &&
                fabs(robotTheta[1] - Configuration::Instance()->limitNeg[1] - 2.0) < 0.1)
        {
            if (preTestCount <= 0)
            {

                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到32
                preTestStatus = 32;

                PRINTF(LOG_DEBUG, "%s: pre-test status at 32, start open value to velocity test.\n", __func__);
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 32:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 计算首个开度
            o2VAimOV = accPositionToOpenValuePositive[0].second + baseOpenValue;

            // 发送控制文件
            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + "/stdand_files/OVC_ARM";
            if( !FileAssistantFunc::IsFileExist(actionFilePath) ){
                return;
            }

            // 开始运动控制
            bool startOk = UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath, ActionMsg::NormalTeach);
            if(!startOk){
                return;
            }

            // 延时设置
            preTestCount = preTestDelayCount * 3;

            // 跳转到33
            preTestStatus = 33;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 33:
    {
        if (preTestCount <= 0)
        {
            double aim = GetPositionFromOpenValue(accPositionToOpenValuePositive[0].second, o2VAimOV, true); // 计算目标
            SendOVCPedalCmd(aim, true, 1);

            // 初始化时间
            o2VStartTime = TimeUtil::CurrentTimestampMs();
            o2VStartTimeInterval = (TimeUtil::CurrentTimestampMs() - o2VStartTime) / 1000.0;

            // 跳转到34
            preTestStatus = 34;

            // 记录数据
            o2VRoundData.clear();
            o2VRoundData.reserve(600);
            o2VRoundData.push_back(std::make_pair(-1.0, -1.0));
        }
        else
        {
            preTestCount--;
            SendOVCPedalCmd(0.0, true, 1);
        }
        break;
    }
    case 34:
    {
        double aim = GetPositionFromOpenValue(accPositionToOpenValuePositive[0].second, o2VAimOV, true); // 计算目标
        SendOVCPedalCmd(aim, true, 1);

        if (std::fabs(robotTheta[1] - aim) < 0.1)
        {
            // 跳转到35
            preTestStatus = 35;
        }

        // 记录数据
        o2VStartTimeInterval = (TimeUtil::CurrentTimestampMs() - o2VStartTime) / 1000.0;
        o2VRoundData.push_back(std::make_pair(o2VStartTimeInterval, pdRobot->GetMP412CarSpeed()));

        break;
    }
    case 35:
    {
        double aim = GetPositionFromOpenValue(accPositionToOpenValuePositive[0].second, o2VAimOV, true); // 计算目标
        SendOVCPedalCmd(aim, true, 1);

        o2VStartTimeInterval = (TimeUtil::CurrentTimestampMs() - o2VStartTime) / 1000.0;
        double speed = pdRobot->GetMP412CarSpeed();
        if (speed >= o2VMaxVelocity || o2VStartTimeInterval >= o2VRoundTime)
        {
            // 跳转到36
            preTestStatus = 36;
            o2VRoundData[0].first = pdRobot->GetAcceleratorOV();
            o2VRoundData[0].second = o2VRoundData[0].first;
        }

        // 记录数据
        o2VRoundData.push_back(std::make_pair(o2VStartTimeInterval, speed));

        break;
    }
    case 36:
    {
        // 测试完毕 缩回踏板
        SendOVCPedalCmd(0.0, true, 1);

        if (std::fabs(robotTheta[1] - Configuration::Instance()->limitNeg[1] - 2.0) < 0.1)
        {
            // 跳转到37
            preTestStatus = 37;
        }
        break;
    }
    case 37:
    {
        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        // 延时设置
        preTestCount = preTestDelayCount;

        // 跳转到38
        preTestStatus = 38;
        break;
    }
    case 38:
    {
        if (preTestCount <= 0)
        {
            if (o2VBrkUsedToDecelerate)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 更新文件 刹车踏板向前
                if (!RefreshPreTestTxt())
                {
                    EndPreTest();
                }

                // 发送action文件
                const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
                UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到39
                preTestStatus = 39;
            }
            else
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                 // 开始等待
                o2VTestPauseForSignal = true;
                emit Signal_O2VWaitBegin(false);

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到43
                preTestStatus = 43;
            }

            // 保存数据
            GetOVToSpeed();
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 39:
    {
        if (fabs(robotTheta[0] - Configuration::Instance()->brakeThetaAfterGoHome) < 0.1)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到40
            preTestStatus = 40;
        }
        break;
    }
    case 40:
    {
        if (pdRobot->GetMP412CarSpeed() < 15.0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 更新文件 刹车踏板向前
            if (!RefreshPreTestTxt())
            {
                EndPreTest();
            }

            // 发送action文件
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到41
            preTestStatus = 41;
        }
        break;
    }
    case 41:
    {
        if (preTestCount <= 0)
        {
            if (pdRobot->GetMP412CarSpeed() <= 2.0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 更新文件 刹车踏板撤回
                if (!RefreshPreTestTxt())
                {
                    EndPreTest();
                }

                // 发送action文件
                const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
                UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到42
                preTestStatus = 42;
            }
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 42:
    {
        if (fabs(robotTheta[0] - Configuration::Instance()->limitNeg[0] - 2.0) < 0.1)
        {
            if (preTestCount <= 0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount * 3 * 5;

                // 跳转到44
                preTestStatus = 44;
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 43:
    {
        if (!o2VTestPauseForSignal)
        {
            if (preTestCount <= 0)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到44
                preTestStatus = 44;
            }
            else
            {
                preTestCount--;
            }
        }
        break;
    }
    case 44:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 查看开度终止条件
            if (o2VAimOV >= accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second)
            {
                // 停止
                UnifiedRobot::Instance()->SendEnterIdleMsg();

                // 延时设置
                preTestCount = preTestDelayCount;

                // 跳转到44
                preTestStatus = 45;

                return;
            }

            // 计算下个开度
            double rangeOV = accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second - accPositionToOpenValuePositive[0].second;
            if (openValueToVelocity.size() == 1)
            {
                double firstOV = accPositionToOpenValuePositive[0].second + ceil(rangeOV / o2VPartNum);
                while (firstOV <= o2VAimOV)
                {
                    firstOV += ceil(rangeOV / o2VPartNum);
                }
                o2VAimOV = firstOV;
            }
            else
            {
                o2VAimOV += ceil(rangeOV / o2VPartNum);
            }
            if (o2VAimOV > accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second)
            {
                o2VAimOV = accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second;
            }

            // 发送控制文件
            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + "/stdand_files/OVC_ARM";
            if( !FileAssistantFunc::IsFileExist(actionFilePath) ){
                return;
            }

            // 开始运动控制
            bool startOk = UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath, ActionMsg::NormalTeach);
            if(!startOk){
                return;
            }

            // 延时设置
            preTestCount = preTestDelayCount * 3;

            // 跳转到33
            preTestStatus = 33;
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    case 45:
    {
        if (preTestCount <= 0)
        {
            // 停止
            UnifiedRobot::Instance()->SendEnterIdleMsg();

            // 保存到文件
            SaveOVToSpeedToTxt();

            // 延时设置
            preTestCount = preTestDelayCount;

            // 跳转到99
            preTestStatus = 99;
        }
        else
        {
            preTestCount--;
        }
        break;
    }

        /*---------------------------------------------------------------------*/

    case 99:
    {
        if (preTestCount <= 0)
        {
            // 判断是否需要存在o2v.txt
            if (!ifTestO2V && QFile::exists(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName)))
            {
                QFile::remove(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName));
            }

            // 更新文件
            if (!RefreshPreTestTxt())
            {
                EndPreTest();
            }

            // 发送action文件
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);

            EndPreTest();
        }
        else
        {
            preTestCount--;
        }
        break;
    }
    default:
        break;
    }
}

void PedalPreTest::EndPreTest()
{
    // 部分标志位还原
    preTestCount = 0;
    preTestStatus = -1;
    o2VTestPauseForSignal = false;
    ifPreTestRun = false;

    // 发送预测试结束信号
    emit Signal_PreTestFinished();

    PRINTF(LOG_DEBUG, "%s: pre-test finished.\n", __func__);
}

void PedalPreTest::ImmediateEndPreTest(bool ifLiftPedal)
{
    if (ifPreTestRun)
    {
        ifPreTestRun = false;

        if (ifLiftPedal)
        {
            // 更新文件
            if (!RefreshPreTestTxt(true))
            {
                return;
            }

            // 踏板抬升
            const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送预测试结束信号
        emit Signal_PreTestFinished();

        PRINTF(LOG_NOTICE, "%s: pre-test closed by user.\n", __func__);
    }
}

bool PedalPreTest::RefreshPreTestTxt(bool ifForceToBeZero)
{
    const std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + preTestFileName;
    SectionInfoPtrList sectionInfoList = SectionInfo::ParseSectionInfoListFromFile(actionFilePath);
    if(sectionInfoList.empty()){
        return false;
    }

    if(sectionInfoList.size() < 2
            || sectionInfoList[0]->GetMode() != RSectionInfo().GetMode()
            || sectionInfoList[1]->GetMode() != TSectionInfo().GetMode()){
        return false;
    }
    TSectionInfo *tsi = dynamic_cast<TSectionInfo*>( sectionInfoList[1].get() );
    if(tsi == nullptr){
        return false;
    }

    if (ifForceToBeZero)
    {
        tsi->transSpeed = Configuration::Instance()->transSpeed * 0.4;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->limitNeg[0] < 0.0 ? 0.0 : Configuration::Instance()->limitNeg[0];
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] < 0.0 ? 0.0 : Configuration::Instance()->limitNeg[1];

        bool stringifyAheadOK = SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, actionFilePath);
        return stringifyAheadOK;
    }

    switch (preTestStatus) {
    case 0:
        tsi->transSpeed = Configuration::Instance()->transSpeed;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->limitNeg[0] < 0.0 ? 0.0 : Configuration::Instance()->limitNeg[0];
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] < 0.0 ? 0.0 : Configuration::Instance()->limitNeg[1];
        break;
    case 2:
        tsi->transSpeed = Configuration::Instance()->transSpeed / 5.0;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
        }
        tsi->theta[preTestAxisNum] = Configuration::Instance()->limitPos[preTestAxisNum];
        break;
    case 4:
        tsi->transSpeed = Configuration::Instance()->transSpeed;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
        }
        tsi->theta[preTestAxisNum] = preTestInitialPedalPosition;
        break;
    case 7:
    case 11:
        tsi->transSpeed = Configuration::Instance()->transSpeed;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = RobotThetaMatrix::Instance()->GetTheta(i);
        }
        tsi->theta[preTestAxisNum] = preTestAimMotorPosition;
        break;

    case 30:
        tsi->transSpeed = Configuration::Instance()->transSpeed;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->limitNeg[0] + 2.0;
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] + 2.0;
        break;
    case 38:
        tsi->transSpeed = Configuration::Instance()->transSpeed / 5.0;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->brakeThetaAfterGoHome;
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] + 2.0;
        break;
    case 40:
        tsi->transSpeed = Configuration::Instance()->transSpeed / 5.0;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->limitPos[0];
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] + 2.0;
        break;
    case 41:
        tsi->transSpeed = Configuration::Instance()->transSpeed;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->limitNeg[0] + 2.0;
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] + 2.0;
        break;

    case 99:
        tsi->transSpeed = Configuration::Instance()->transSpeed;
        for(int i=0; i<RobotParams::axisNum; ++i){
            tsi->theta[i] = 0.0;
        }
        tsi->theta[0] = Configuration::Instance()->limitNeg[0] + 2.0;
        tsi->theta[1] = Configuration::Instance()->limitNeg[1] + 2.0;
        break;
    default:
        break;
    }

    bool stringifyOK = SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, actionFilePath);
    return stringifyOK;
}

void PedalPreTest::GetBrkPosToOV(bool ifPositive)
{
    if (ifPositive)
    {
        brkPositionToOpenValuePositive.clear();
        brkPositionToOpenValuePositive.reserve(100);
    }
    else
    {
        brkPositionToOpenValueNegative.clear();
        brkPositionToOpenValueNegative.reserve(100);
    }

    int startNum = 0, endNum = 0;
    if (ifPositive)
    {
        for (size_t k = 0; k < preTestDatas.size(); k++)
        {
            if (round(preTestDatas[k].second * 100) > round(preTestDatas[0].second * 100))
            {
                startNum = k - 1;
                break;
            }
        }
    }
    else
    {
        for (size_t k = 0; k < preTestDatas.size(); k++)
        {
            if (round(preTestDatas[k].second * 100) <= round(preTestDatas[preTestDatas.size() - 1].second * 100))
            {
                endNum = k;
                break;
            }
        }
    }

    if (ifPositive)
    {
        for (size_t k = startNum; k < preTestDatas.size(); k++)
        {
            brkPositionToOpenValuePositive.push_back(std::make_pair(preTestDatas[k].first, preTestDatas[k].second));
        }
    }
    else
    {
        for (int k = 0; k <= endNum; k++)
        {
            brkPositionToOpenValueNegative.push_back(std::make_pair(preTestDatas[k].first, preTestDatas[k].second));
        }
    }
}

void PedalPreTest::GetAccPosToOV(bool ifPositive)
{
    if (ifPositive)
    {
        accPositionToOpenValuePositive.clear();
        accPositionToOpenValuePositive.reserve(100);
    }
    else
    {
        accPositionToOpenValueNegative.clear();
        accPositionToOpenValueNegative.reserve(100);
    }

    double eps = 1e-10;
    double preTestDataFirstOV = round(preTestDatas[0].second * 100.0) / 100.0;
    double preTestDataLastOV = round(preTestDatas[preTestDatas.size() - 1].second * 100.0) / 100.0;

    size_t startNum = 0, endNum = 0;
    if (ifPositive)
    {
        for (size_t k = 1; k < preTestDatas.size(); k++)
        {
            double preTestDataCurrentOV = round(preTestDatas[k].second * 100.0) / 100.0;

            if (preTestDataCurrentOV > preTestDataFirstOV)
            {
                startNum = k - 1;
                break;
            }
        }
        for (size_t k = preTestDatas.size() - 2; k > startNum; k--)
        {
            double preTestDataCurrentOV = round(preTestDatas[k].second * 100.0) / 100.0;

            if (preTestDataCurrentOV < preTestDataLastOV)
            {
                endNum = k + 1;
                break;
            }
        }
    }
    else
    {
        for (size_t k = 1; k < preTestDatas.size(); k++)
        {
            double preTestDataCurrentOV = round(preTestDatas[k].second * 100.0) / 100.0;

            if (preTestDataCurrentOV < preTestDataFirstOV)
            {
                startNum = k - 1;
                break;
            }
        }
        for (size_t k = preTestDatas.size() - 2; k > startNum; k--)
        {
            double preTestDataCurrentOV = round(preTestDatas[k].second * 100.0) / 100.0;

            if (preTestDataCurrentOV > preTestDataLastOV)
            {
                endNum = k + 1;
                break;
            }
        }
    }

    if (ifPositive)
    {
        double preTestDataStartPos = round(preTestDatas[startNum].first * 100.0) / 100.0;
        double preTestDataStartOV = round(preTestDatas[startNum].second * 100.0) / 100.0;
        accPositionToOpenValuePositive.push_back(std::make_pair(preTestDataStartPos, preTestDataStartOV));

        for (size_t k = startNum + 1; k <= endNum; k++)
        {
            double preTestDataNowPos = round(preTestDatas[k].first * 100.0) / 100.0;
            double preTestDataNowOV = round(preTestDatas[k].second * 100.0) / 100.0;

            if (fabs(preTestDataNowOV - accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second) < eps)
            {
                accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].first /= 2.0;
                accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].first += (preTestDataNowPos / 2.0);
            }
            else
            {
                accPositionToOpenValuePositive.push_back(std::make_pair(preTestDataNowPos, preTestDataNowOV));
            }
        }
    }
    else
    {
        double preTestDataStartPos = round(preTestDatas[startNum].first * 100.0) / 100.0;
        double preTestDataStartOV = round(preTestDatas[startNum].second * 100.0) / 100.0;
        accPositionToOpenValueNegative.push_back(std::make_pair(preTestDataStartPos, preTestDataStartOV));

        for (size_t k = startNum + 1; k <= endNum; k++)
        {
            double preTestDataNowPos = round(preTestDatas[k].first * 100.0) / 100.0;
            double preTestDataNowOV = round(preTestDatas[k].second * 100.0) / 100.0;

            if (fabs(preTestDataNowOV - accPositionToOpenValueNegative[accPositionToOpenValueNegative.size() - 1].second) < eps)
            {
                accPositionToOpenValueNegative[accPositionToOpenValueNegative.size() - 1].first /= 2.0;
                accPositionToOpenValueNegative[accPositionToOpenValueNegative.size() - 1].first += (preTestDataNowPos / 2.0);
            }
            else
            {
                accPositionToOpenValueNegative.push_back(std::make_pair(preTestDataNowPos, preTestDataNowOV));
            }
        }
    }
}

void PedalPreTest::SaveConfParams(bool ifStart)
{
    // 更新conf文件
    if (ifStart)
    {
        if (ifAutoBrkLimitNeg)
        {
            Configuration::Instance()->limitNeg[0] = - 2.0;
        }
        if(ifAutoAccLimitNeg)
        {
            Configuration::Instance()->limitNeg[1] = - 2.0;
        }
    }
    else
    {
        Configuration::Instance()->limitNeg[0] = std::min(brkPositionToOpenValuePositive[0].first, brkPositionToOpenValueNegative[brkPositionToOpenValueNegative.size() - 1].first) - 2.0;
        Configuration::Instance()->limitNeg[1] = std::min(accPositionToOpenValuePositive[0].first, accPositionToOpenValueNegative[accPositionToOpenValueNegative.size() - 1].first) - 2.0;
    }
    Configuration::Instance()->SaveToFile();

    // 发送conf到下位机
    UnifiedRobot::Instance()->SendConfiguration();
}

void PedalPreTest::SavePosToOVToTxt()
{
    std::string p2OPath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + posToOVFileName;
    std::string p2OContent = "";

    p2OContent += "brk_positive\n";
    for (size_t k = 0; k < brkPositionToOpenValuePositive.size(); k++)
    {
        p2OContent += QString::number(brkPositionToOpenValuePositive[k].first, 'f', 2).toStdString() + " " +
                QString::number(brkPositionToOpenValuePositive[k].second, 'f', 2).toStdString() + "\n";
    }
    p2OContent += "brk_negative\n";
    for (size_t k = 0; k < brkPositionToOpenValueNegative.size(); k++)
    {
        p2OContent += QString::number(brkPositionToOpenValueNegative[k].first, 'f', 2).toStdString() + " " +
                QString::number(brkPositionToOpenValueNegative[k].second, 'f', 2).toStdString() + "\n";
    }
    p2OContent += "acc_positive\n";
    for (size_t k = 0; k < accPositionToOpenValuePositive.size(); k++)
    {
        p2OContent += QString::number(accPositionToOpenValuePositive[k].first, 'f', 2).toStdString() + " " +
                QString::number(accPositionToOpenValuePositive[k].second, 'f', 2).toStdString() + "\n";
    }
    p2OContent += "acc_negative\n";
    for (size_t k = 0; k < accPositionToOpenValueNegative.size(); k++)
    {
        p2OContent += QString::number(accPositionToOpenValueNegative[k].first, 'f', 2).toStdString() + " " +
                QString::number(accPositionToOpenValueNegative[k].second, 'f', 2).toStdString() + "\n";
    }

    NormalFile::WriteToFile(p2OPath.c_str(), p2OContent.c_str(), p2OContent.size());
    PRINTF(LOG_DEBUG, "%s: %s saved.\n", __func__, p2OPath.c_str());

    if (ifAutoTransfer)
    {
        QString str;
        TransferP2OToAnotherPath(str);
    }
}

void PedalPreTest::ReadSavedPreTestFile()
{
    // Read p2o
    std::string p2OPath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + posToOVFileName;

    QString p2ODatas;
    if (NormalFile::ReadAllContents(p2OPath.c_str(), p2ODatas) < 0)
    {
        return;
    }

    brkPositionToOpenValuePositive.clear();
    brkPositionToOpenValuePositive.reserve(100);
    brkPositionToOpenValueNegative.clear();
    brkPositionToOpenValueNegative.reserve(100);
    accPositionToOpenValuePositive.clear();
    accPositionToOpenValuePositive.reserve(100);
    accPositionToOpenValueNegative.clear();
    accPositionToOpenValueNegative.reserve(100);

    int status = 0;
    QStringList p2ODataList = p2ODatas.split('\n', QString::SkipEmptyParts);
    for (int k = 0; k < p2ODataList.size(); k++)
    {
        if ((QString)p2ODataList.at(k) == "brk_positive") status = 0;
        else if ((QString)p2ODataList.at(k) == "brk_negative") status = 1;
        else if ((QString)p2ODataList.at(k) == "acc_positive") status = 2;
        else if ((QString)p2ODataList.at(k) == "acc_negative") status = 3;
        else
        {
            QStringList item = ((QString)p2ODataList.at(k)).split(" ");
            switch (status) {
            case 0:
                brkPositionToOpenValuePositive.push_back(std::make_pair(((QString)item.at(0)).toDouble(),  ((QString)item.at(1)).toDouble()));
                break;
            case 1:
                brkPositionToOpenValueNegative.push_back(std::make_pair(((QString)item.at(0)).toDouble(),  ((QString)item.at(1)).toDouble()));
                break;
            case 2:
                accPositionToOpenValuePositive.push_back(std::make_pair(((QString)item.at(0)).toDouble(),  ((QString)item.at(1)).toDouble()));
                break;
            case 3:
                accPositionToOpenValueNegative.push_back(std::make_pair(((QString)item.at(0)).toDouble(),  ((QString)item.at(1)).toDouble()));
                break;
            default:
                break;
            }
        }
    }

    // Read o2v
    std::string o2VPath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName;

    QString o2VDatas;
    if (NormalFile::ReadAllContents(o2VPath.c_str(), o2VDatas) < 0)
    {
        return;
    }

    openValueToVelocity.clear();
    openValueToVelocity.reserve(30);

    std::vector< std::pair<double, double> > rdData;
    rdData.clear();
    rdData.reserve(600);
    QStringList o2VDataList = o2VDatas.split('\n', QString::SkipEmptyParts);
    rdData.push_back(std::make_pair(((QString)o2VDataList.at(0)).mid(3).toDouble(), ((QString)o2VDataList.at(0)).mid(3).toDouble()));
    for (int k = 1; k < o2VDataList.size(); k++)
    {
        if (((QString)o2VDataList.at(k)).left(2)  == "ov")
        {
            openValueToVelocity.push_back(std::vector< std::pair<double, double> >(rdData));
            rdData.clear();
            rdData.reserve(600);
            rdData.push_back(std::make_pair(((QString)o2VDataList.at(k)).mid(3).toDouble(), ((QString)o2VDataList.at(k)).mid(3).toDouble()));
        }
        else
        {
            QStringList timeAndSpeed = ((QString)o2VDataList.at(k)).split(' ', QString::SkipEmptyParts);
            rdData.push_back(std::make_pair(((QString)timeAndSpeed.at(0)).toDouble(), ((QString)timeAndSpeed.at(1)).toDouble()));
        }
    }
    openValueToVelocity.push_back(std::vector< std::pair<double, double> >(rdData));

    // Save to configuration
    SaveConfParams();
}

double PedalPreTest::GetPositionFromOpenValue(double currentOpenValue, double aimOpenValue, bool ifAccAxis)
{
    if (ifAccAxis)
    {
        // 限幅
        if (aimOpenValue < accPositionToOpenValuePositive[0].second)
        {
            aimOpenValue = accPositionToOpenValuePositive[0].second;
        }
        if (aimOpenValue > accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second)
        {
            aimOpenValue = accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second;
        }

        // 确定踏板移动方向
        bool ifPos = (aimOpenValue - currentOpenValue) >= 0.0;

        // 确定上下界值
        int upperPosNum = 0, lowerPosNum = 0;
        if (ifPos)
        {
            for (size_t k = 0; k < accPositionToOpenValuePositive.size(); k++)
            {
                if (aimOpenValue < accPositionToOpenValuePositive[k].second)
                {
                    upperPosNum = k;
                    lowerPosNum = upperPosNum - 1;
                    break;
                }
            }
            if (upperPosNum == lowerPosNum)
            {
                upperPosNum = accPositionToOpenValuePositive.size() - 1;
                lowerPosNum = upperPosNum - 1;
            }

            // 一维线性插值
            return OneDimensionInterpolation(accPositionToOpenValuePositive[lowerPosNum].second, accPositionToOpenValuePositive[lowerPosNum].first,
                                                                    accPositionToOpenValuePositive[upperPosNum].second, accPositionToOpenValuePositive[upperPosNum].first,
                                                                    aimOpenValue);
        }
        else
        {
            for (size_t k = 0; k < accPositionToOpenValueNegative.size(); k++)
            {
                if (aimOpenValue > accPositionToOpenValueNegative[k].second)
                {
                    upperPosNum = k;
                    lowerPosNum = upperPosNum - 1;
                    break;
                }
            }
            if (upperPosNum == lowerPosNum)
            {
                upperPosNum = accPositionToOpenValueNegative.size() - 1;
                lowerPosNum = upperPosNum - 1;
            }

            // 一维线性插值
            return OneDimensionInterpolation(accPositionToOpenValueNegative[lowerPosNum].second, accPositionToOpenValueNegative[lowerPosNum].first,
                                                                    accPositionToOpenValueNegative[upperPosNum].second, accPositionToOpenValueNegative[upperPosNum].first,
                                                                    aimOpenValue);
        }
    }
    else
    {
        // 限幅
        if (aimOpenValue < brkPositionToOpenValuePositive[0].second)
        {
            aimOpenValue = brkPositionToOpenValuePositive[0].second;
        }
        if (aimOpenValue > brkPositionToOpenValuePositive[brkPositionToOpenValuePositive.size() - 1].second)
        {
            aimOpenValue = brkPositionToOpenValuePositive[brkPositionToOpenValuePositive.size() - 1].second;
        }

        // 确定踏板移动方向
        bool ifPos = (aimOpenValue - currentOpenValue) >= 0.0;

        // 确定上下界值
        int upperPosNum = 0, lowerPosNum = 0;
        if (ifPos)
        {
            for (size_t k = 0; k < brkPositionToOpenValuePositive.size(); k++)
            {
                if (aimOpenValue < brkPositionToOpenValuePositive[k].second)
                {
                    upperPosNum = k;
                    lowerPosNum = upperPosNum - 1;
                    break;
                }
            }
            if (upperPosNum == lowerPosNum)
            {
                upperPosNum = brkPositionToOpenValuePositive.size() - 1;
                lowerPosNum = upperPosNum - 1;
            }

            // 一维线性插值
            return OneDimensionInterpolation(brkPositionToOpenValuePositive[lowerPosNum].second, brkPositionToOpenValuePositive[lowerPosNum].first,
                                                                    brkPositionToOpenValuePositive[upperPosNum].second, brkPositionToOpenValuePositive[upperPosNum].first,
                                                                    aimOpenValue);
        }
        else
        {
            for (size_t k = 0; k < brkPositionToOpenValueNegative.size(); k++)
            {
                if (aimOpenValue > brkPositionToOpenValueNegative[k].second)
                {
                    upperPosNum = k;
                    lowerPosNum = upperPosNum - 1;
                    break;
                }
            }
            if (upperPosNum == lowerPosNum)
            {
                upperPosNum = brkPositionToOpenValueNegative.size() - 1;
                lowerPosNum = upperPosNum - 1;
            }

            // 一维线性插值
            return OneDimensionInterpolation(brkPositionToOpenValueNegative[lowerPosNum].second, brkPositionToOpenValueNegative[lowerPosNum].first,
                                                                    brkPositionToOpenValueNegative[upperPosNum].second, brkPositionToOpenValueNegative[upperPosNum].first,
                                                                    aimOpenValue);
        }
    }
}




double PedalPreTest::GetLimitOpenValue(int axisNum, bool ifMax)
{
    if (axisNum == 0)
    {
        if (brkPositionToOpenValuePositive.size() == 0)
        {
            return 0.0;
        }

        if (ifMax)
        {
            return brkPositionToOpenValuePositive[brkPositionToOpenValuePositive.size() - 1].second;
        }
        else
        {
            return brkPositionToOpenValuePositive[0].second;
        }
    }
    else
    {
        if (accPositionToOpenValuePositive.size() == 0)
        {
            return 0.0;
        }

        if (ifMax)
        {
            return accPositionToOpenValuePositive[accPositionToOpenValuePositive.size() - 1].second;
        }
        else
        {
            return accPositionToOpenValuePositive[0].second;
        }
    }
}

void PedalPreTest::TransferFileToAnotherPath(QString filePath)
{
    TransferP2OToAnotherPath(filePath);
    TransferO2VToAnotherPath(filePath);
}

void PedalPreTest::PickTransferFileFromAnotherPath(QString filePath)
{
    if (filePath.isEmpty()) return;

    PickTransferP2OFileFromAnotherPath(filePath);

    filePath.remove(filePath.size() - 7, 7);
    filePath.insert(filePath.size() - 1, "o2v.txt");
    PickTransferO2VFileFromAnotherPath(filePath);
}

double PedalPreTest::GetOVFromTimeAndSpeed(double time, double speed)
{
    if (openValueToVelocity.size() < 1) return GetLimitOpenValue(1, false);
    if (time > 30.0) return GetLimitOpenValue(1, false);
    if (speed < 0 || speed > 140.0) return GetLimitOpenValue(1, false);

    std::vector< std::pair<double, double> > baseTimeAndSpeed = openValueToVelocity.at(0);
    size_t baseTimeNum = 2;
    for (size_t t = 2; t < baseTimeAndSpeed.size(); t++)
    {
        if (baseTimeAndSpeed[t].first >= time)
        {
            baseTimeNum = t;
            break;
        }
    }
    double coeffForBaseLowTime = (baseTimeAndSpeed[baseTimeNum].first - time) / (baseTimeAndSpeed[baseTimeNum].first - baseTimeAndSpeed[baseTimeNum - 1].first);
    double lastActualSpeed = baseTimeAndSpeed[baseTimeNum - 1].second * coeffForBaseLowTime + baseTimeAndSpeed[baseTimeNum].second * (1.0 - coeffForBaseLowTime);

    double thisActualSpeed = 0.0;
    size_t keySeriesNum = 1;
    for (size_t k = 1; k < openValueToVelocity.size(); k++)
    {
        std::vector< std::pair<double, double> > tempTimeAndSpeed = openValueToVelocity.at(k);
        size_t keyTimeNum = 2;
        for (size_t t = 2; t < tempTimeAndSpeed.size(); t++)
        {
            if (tempTimeAndSpeed[t].first >= time)
            {
                keyTimeNum = t;
                break;
            }
        }
        double coeffForLowTime = (tempTimeAndSpeed[keyTimeNum].first - time) / (tempTimeAndSpeed[keyTimeNum].first - tempTimeAndSpeed[keyTimeNum - 1].first);
        double actualSpeed = tempTimeAndSpeed[keyTimeNum - 1].second * coeffForLowTime + tempTimeAndSpeed[keyTimeNum].second * (1.0 - coeffForLowTime);

        if (actualSpeed > speed)
        {
            keySeriesNum = k;
            thisActualSpeed = actualSpeed;
            break;
        }
        else
        {
            lastActualSpeed = actualSpeed;
        }
    }

    if (keySeriesNum >= openValueToVelocity.size())
    {
        return GetLimitOpenValue(1, true);
    }
    else
    {
        int upOVSeriesNum = (int)keySeriesNum;
        int lowOVSeriesNum = upOVSeriesNum - 1;
        double upOV = ((std::vector<std::pair<double,double>>)openValueToVelocity.at(upOVSeriesNum))[0].first;
        double lowOV = ((std::vector<std::pair<double,double>>)openValueToVelocity.at(lowOVSeriesNum))[0].first;
        double upSpeed = thisActualSpeed;
        double lowSpeed = lastActualSpeed;

        double coeffLowOV = (upSpeed - speed) / (upSpeed - lowSpeed);
        double givenOV = lowOV * coeffLowOV + upOV * (1 - coeffLowOV);
        givenOV = givenOV > GetLimitOpenValue(1, true) ? GetLimitOpenValue(1, true) : givenOV;
        givenOV = givenOV < GetLimitOpenValue(1, false) ? GetLimitOpenValue(1, false) : givenOV;

        return givenOV;
    }
}

void PedalPreTest::SendOVCPedalCmd(double value, bool ifAbs, int axisNum)
{
    std::vector<int> actionAxes(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionAxes[i] = i;
    }

#ifdef ENABLE_MOREMOTORMODE
    std::vector<int> actionMethod(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionMethod[i] = MsgStructure::MonitorCommand::AbsControl;
    }
    if (!ifAbs)
    {
        actionMethod[axisNum] = MsgStructure::MonitorCommand::DeltaControl;
    }

    std::vector<double> actionTheta(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionTheta[i] = 0.0;
    }
    actionTheta[axisNum] = value;

    UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
#else
    int method = 0;
    if (ifAbs)
    {
        method = MsgStructure::MonitorCommand::AbsControl;

        std::vector<int> actionMethod(RobotParams::axisNum);
        for(int i=0; i<RobotParams::axisNum; ++i){
            actionMethod[i] = method;
        }

        std::vector<double> actionTheta(RobotParams::axisNum);
        for(int i=0; i<RobotParams::axisNum; ++i){
            actionTheta[i] = 0.0;
        }
        actionTheta[axisNum] = value;

        UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
    }
    else
    {
        method = MsgStructure::MonitorCommand::DeltaControl;

        std::vector<int> actionMethod(RobotParams::axisNum);
        for(int i=0; i<RobotParams::axisNum; ++i){
            actionMethod[i] = method;
        }

        std::vector<double> actionTheta(RobotParams::axisNum);
        for(int i=0; i<RobotParams::axisNum; ++i){
            actionTheta[i] = -100.0;
        }
        actionTheta[axisNum] = value;

        UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
    }
#endif
}

void PedalPreTest::GetOVToSpeed()
{
    std::vector< std::pair<double, double> > temp(o2VRoundData);
    openValueToVelocity.push_back(temp);
}

void PedalPreTest::SaveOVToSpeedToTxt()
{
    std::string o2VPath = Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName;
    std::string o2VContent = "";

    for (size_t h = 0; h < openValueToVelocity.size(); h++)
    {
        std::vector< std::pair<double, double> > tempV = openValueToVelocity.at(h);
        o2VContent += ("ov " + QString::number(tempV[0].first, 'f', 1).toStdString() + "\n");
        for (size_t k = 1; k < tempV.size(); k++)
        {
            o2VContent += QString::number(tempV[k].first, 'f', 3).toStdString() + " " +
                    QString::number(tempV[k].second, 'f', 2).toStdString() + "\n";
        }
    }

    NormalFile::WriteToFile(o2VPath.c_str(), o2VContent.c_str(), o2VContent.size());
    PRINTF(LOG_DEBUG, "%s: %s saved.\n", __func__, o2VPath.c_str());

    if (ifAutoTransfer)
    {
        QString str;
        TransferO2VToAnotherPath(str);
    }
}

void PedalPreTest::TransferP2OToAnotherPath(QString filePath)
{
    if (filePath.isEmpty())
    {
        QString transferFileName = QDateTime::currentDateTime().date().toString("yyyy-MM-dd") + " " +
                QString::number(QTime::currentTime().hour()) + "-" +
                QString::number(QTime::currentTime().minute()) + "-" +
                QString::number(QTime::currentTime().second());

        transferFileHead = transferFileName;

        filePath = QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + transferDirectoryP2O + (transferFileName + " p2o.txt").toStdString());
    }
    else
    {
        if (filePath.right(4) == ".txt")
        {
            filePath.remove(filePath.size() - 4, 4);
        }
        filePath.insert(filePath.size() - 1, " p2o.txt");
    }

    if (QFile::copy(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + posToOVFileName), filePath))
    {
        PRINTF(LOG_DEBUG, "%s: Transfer file %s saved.\n", __func__, filePath.toStdString().c_str());
    }
}

void PedalPreTest::TransferO2VToAnotherPath(QString filePath)
{
    if (filePath.isEmpty())
    {
        QString transferFileName = transferFileHead + " o2v.txt";

        filePath = QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + transferDirectoryO2V + transferFileName.toStdString());
    }
    else
    {
        if (filePath.right(4) == ".txt")
        {
            filePath.remove(filePath.size() - 4, 4);
        }
        filePath.insert(filePath.size() - 1, " o2v.txt");
    }

    if (QFile::copy(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName), filePath))
    {
        PRINTF(LOG_DEBUG, "%s: Transfer file %s saved.\n", __func__, filePath.toStdString().c_str());
    }
}

void PedalPreTest::PickTransferP2OFileFromAnotherPath(QString filePath)
{
    if (filePath.isEmpty()) return;

    if (QFile::exists(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + posToOVFileName)))
    {
        if (!QFile::remove(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + posToOVFileName)))
        {
            return;
        }
    }

    if (QFile::copy(filePath, QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + posToOVFileName)))
    {
        PRINTF(LOG_DEBUG, "%s: Transfer file %s repicked.\n", __func__, filePath.toStdString().c_str());
    }
}

void PedalPreTest::PickTransferO2VFileFromAnotherPath(QString filePath)
{
    if (filePath.isEmpty()) return;

    if (QFile::exists(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName)))
    {
        if (!QFile::remove(QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName)))
        {
            return;
        }
    }

    if (QFile::copy(filePath, QString::fromStdString(Configuration::Instance()->GetFilePath(Configuration::SystemFolderPath) + ovToVelocityFileName)))
    {
        PRINTF(LOG_DEBUG, "%s: Transfer file %s repicked.\n", __func__, filePath.toStdString().c_str());
    }
}

double PedalPreTest::OneDimensionInterpolation(double x1, double y1, double x2, double y2, double x)
{
    double eps = 1e-10;
    if (fabs(x2 - x1) < eps)
    {
        return (y1 + y2) / 2.0;
    }
    else
    {
        return (x - x1) / (x2 - x1) * (y2 - y1) + y1;
    }
}


