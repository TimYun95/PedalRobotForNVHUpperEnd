#include "pedalrobotnvh.h"

#include "miscconfigurationparam.h"

#include "mywidget/mymessagebox.h"

#include "common/printf.h"

#include "fileoperation/normalfile.h"
#include "assistantfunc/fileassistantfunc.h"

#include "util/timeutil.h"
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
PedalRobotNVH::PedalRobotNVH(QObject *parent)
    : QObject(parent),
      recordNVHDataDirectory("/nvh_files/"),
      NVH_DASFP_VERYLOWTIME(12.5), NVH_DASFP_VERYLOWSPEED(22.0),
      NVH_DASFP_LOWTIME(18.5), NVH_DASFP_LOWSPEED(60.0),
      NVH_DASFP_MODERATETIME(23.5), NVH_DASFP_MODERATESPEED(100.0),
      NVH_DASCT_VERYLOWTIME(60.0), NVH_DASCT_VERYLOWMAXSPEED(50.7),
      NVH_DASCT_LOWTIME(90.0), NVH_DASCT_LOWMAXSPEED(123.9),
      NVH_DASCT_MODERATETIME(50.0), NVH_DASCT_MODERATEMAXSPEED(141.7),
      NVH_CS_ACCELERATION(0.7), NVH_CS_TERMINATEDSPEED(140.0),
      caseChangeStopInterval(6),
      dvAlpha(0.25),
      openLoopTime(0.3),
      positionError(0.1),
      openValueError(0.1),
      overTimeInterval(3.0),
      leastTimeInterval(1.8),
      waitTimeInterval(100.0)
{
    // DAS-CT曲线载入
    InsertCurveTemplateDASCT();

    // 计算程序运行时间
    programmaStartTime = TimeUtil::CurrentTimestampMs();
    programmaStartTimeInterval = (TimeUtil::CurrentTimestampMs() - programmaStartTime) / 1000.0;

    // 初始化URI
    pUri = new URI(Q_NULLPTR, false, false);//关闭URI内部的定时器
#ifdef PEDAL_ROBOT_API_DISABLE_MESSAGE_BOX
    bool enableShowBoxWhenFinishGoHome = false;
    bool enablePedalSystemControl = false;
#else
    bool enableShowBoxWhenFinishGoHome = true;
    bool enablePedalSystemControl = true;
#endif
    MiscConfigurationParam::Instance()->SetConfigParam(MiscConfigurationParam::eEnableShowBoxWhenFinishGoHome, enableShowBoxWhenFinishGoHome);
    MiscConfigurationParam::Instance()->SetConfigParam(MiscConfigurationParam::eEnablePedalSystemControl, enablePedalSystemControl);

    // 初始化USB数据更新类
    pdUSB = new PedalRobotUSBProcessor();

    // 初始化预测试类
    pdPreTest = new PedalPreTest(pdUSB);

    InitSignalSlot(); // 信号槽初始化
    InitParams(); // 参数初始化
}


PedalRobotNVH::~PedalRobotNVH()
{
    delete pdPreTest;
    delete pdUSB;
    delete pUri;
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
URI* PedalRobotNVH::GetURI()
{
    return pUri;
}

void PedalRobotNVH::StopAllModuleMotion()
{
    pdPreTest->ImmediateEndPreTest(false);
    ImmediateEndNVHCtrl(false);
    NotImmediateEndNVHCtrl();
}

void PedalRobotNVH::StartInternalTimer()
{
    LogicUpdateTimer.StartTimer(RobotParams::logicUpdateTimer_IntervalMs, RobotParams::logicUpdateTimer_IntervalMultiplier);
    WidgetUpdateTimer.StartTimer(RobotParams::widgetUpdateTimer_IntervalMs, RobotParams::widgetUpdateTimer_IntervalMultiplier);
}

void PedalRobotNVH::StopInternalTimer()
{
    LogicUpdateTimer.StopTimer();
    WidgetUpdateTimer.StopTimer();
}

bool PedalRobotNVH::GetPreTestStatus()
{
    return pdPreTest->ifPreTestRun;
}

std::string PedalRobotNVH::GetP2OTransferDirectory()
{
    return pdPreTest->transferDirectoryP2O;
}

std::string PedalRobotNVH::GetO2VTransferDirectory()
{
    return pdPreTest->transferDirectoryO2V;
}

bool PedalRobotNVH::GetO2VTestStatus()
{
    return pdPreTest->o2VTestPauseForSignal;
}

int PedalRobotNVH::GetNVHStatus()
{
    return statusNVHRun;
}
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
void PedalRobotNVH::InsertCurveTemplateDASCT()
{
    // VL
    NVH_DASCT_VERYLOWCURVE.clear();
    NVH_DASCT_VERYLOWCURVE.reserve(13);
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(0.0, 8.0));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(5.0, 14.4));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(10.0, 19.7));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(15.0, 24.4));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(20.0, 28.5));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(25.0, 32.2));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(30.0, 35.5));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(35.0, 38.6));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(40.0, 41.4));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(45.0, 43.9));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(50.0, 46.4));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(55.0, 48.6));
    NVH_DASCT_VERYLOWCURVE.push_back(std::make_pair(60.0, 50.7));

    // L
    NVH_DASCT_LOWCURVE.clear();
    NVH_DASCT_LOWCURVE.reserve(19);
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(0.0, 8.0));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(5.0, 27.7));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(10.0, 42.2));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(15.0, 53.6));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(20.0, 62.7));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(25.0, 70.6));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(30.0, 77.3));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(35.0, 83.3));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(40.0, 88.6));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(45.0, 93.5));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(50.0, 97.9));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(55.0, 102.0));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(60.0, 105.8));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(65.0, 109.3));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(70.0, 112.6));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(75.0, 115.6));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(80.0, 118.5));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(85.0, 121.3));
    NVH_DASCT_LOWCURVE.push_back(std::make_pair(90.0, 123.9));

    // M
    NVH_DASCT_MODERATECURVE.clear();
    NVH_DASCT_MODERATECURVE.reserve(11);
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(0.0, 8.0));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(5.0, 40.6));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(10.0, 62.5));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(15.0, 79.0));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(20.0, 92.3));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(25.0, 103.4));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(30.0, 113.0));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(35.0, 121.3));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(40.0, 128.8));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(45.0, 135.5));
    NVH_DASCT_MODERATECURVE.push_back(std::make_pair(50.0, 141.7));
}

void PedalRobotNVH::InitSignalSlot()
{
    connect(&LogicUpdateTimer, SIGNAL(intervalTimeout(int,int)), this, SLOT(Slot_LogicUpdateTimeOut(int,int)));
    connect(&WidgetUpdateTimer, SIGNAL(timeout()), this, SLOT(Slot_WidgetUpdateTimeOut()));

    connect(pdPreTest, SIGNAL(Signal_PreTestFinished()), this, SLOT(Slot_PreTestFinished()));
    connect(pdPreTest, SIGNAL(Signal_O2VWaitBegin(bool)), this, SLOT(Slot_O2VWaitBegin(bool)));
}

void PedalRobotNVH::InitParams()
{
    caseNVH = 1;
    caseChangeStopIntervalCount = caseChangeStopInterval;
    statusNVHRun = 0;
    startNVHTime = 0.0;
    startNVHTimeInterval = 0.0;
    intervalNVHTimeInterval = 0.054;
    startNVHAccOV = 0.0;
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);

    ifRun_NVH_DASFP = false;
    ifRun_NVH_DASCT = false;
    ifRun_NVH_CS_TestOV = false;
    ifRun_NVH_CS = false;
    ifRun_NVH_HLGS_KeepSpeed = false;
    ifRun_NVH_HLGS = false;
    ifRun_NVH_PGS = false;
    ifRun_NVH_APS = false;
}

void PedalRobotNVH::UpdateLogicControl()
{
    UpdateLocalLogic();
    pdPreTest->UpdatePreTestLogic();
}

void PedalRobotNVH::UpdateLocalLogic()
{
    programmaStartTimeInterval = (TimeUtil::CurrentTimestampMs() - programmaStartTime) / 1000.0;
    UpdateNVHLogic();
}

bool PedalRobotNVH::ResetOriginFileByDeathValue()
{
    const std::string originFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotOriginFilePath);
    SectionInfoPtrList sectionInfoList = SectionInfo::ParseSectionInfoListFromFile(originFilePath);
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

    tsi->transSpeed = Configuration::Instance()->transSpeed;
    for(int i=0; i<RobotParams::axisNum; ++i){
        tsi->theta[i] = 0.0;
    }
    tsi->theta[0] = Configuration::Instance()->brakeThetaAfterGoHome;//刹车 运动到死区页面设置的回原后位置

    bool stringifyOK = SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, originFilePath);
    return stringifyOK;
}

bool PedalRobotNVH::ResetSoftStopFile()
{
    const std::string softStopFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
    SectionInfoPtrList sectionInfoList = SectionInfo::ParseSectionInfoListFromFile(softStopFilePath);
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

    tsi->transSpeed = Configuration::Instance()->transSpeed * 0.4;
    for(int i=0; i<RobotParams::axisNum; ++i){
        tsi->theta[i] = 0.0;
    }
    tsi->theta[0] = Configuration::Instance()->limitNeg[0] < 0.0 ? 0.0 : Configuration::Instance()->limitNeg[0];
    tsi->theta[1] = Configuration::Instance()->limitNeg[1] < 0.0 ? 0.0 : Configuration::Instance()->limitNeg[1];

    bool stringifyOK = SectionInfo::StringifySectionInfoListIntoFile(sectionInfoList, softStopFilePath);
    return stringifyOK;
}

PedalRobotNVH::paraMap PedalRobotNVH::UpdateTransferParams(int * const dataLength)
{
    double prgTime = (TimeUtil::CurrentTimestampMs() - programmaStartTime) / 1000.0;
    paraMap transferParams;
    transferParams.clear();
    transferParams.insert("prgTime", prgTime);
    transferParams.insert("brkPos", RobotThetaMatrix::Instance()->GetTheta(0));
    transferParams.insert("accPos", RobotThetaMatrix::Instance()->GetTheta(1));
    transferParams.insert("brkOV", pdUSB->GetBrakeOV());
    transferParams.insert("accOV", pdUSB->GetAcceleratorOV());
    transferParams.insert("canSpeed", pdUSB->GetCanCarSpeed());
    transferParams.insert("pulseSpeed", pdUSB->GetMP412CarSpeed());
    transferParams.insert("powerMode", pdUSB->GetPowerMode());
    transferParams.insert("adVoltage", pdUSB->GetAdVoltage());
    transferParams.insert("pulseFrequency", pdUSB->GetPulseFrequency());
    transferParams.insert("goHome", goHomeState);
    transferParams.insert("brkOVUB", pdPreTest->GetLimitOpenValue(0, true));
    transferParams.insert("brkOVLB", pdPreTest->GetLimitOpenValue(0, false));
    transferParams.insert("accOVUB", pdPreTest->GetLimitOpenValue(1, true));
    transferParams.insert("accOVLB", pdPreTest->GetLimitOpenValue(1, false));

    if (recordDatasNVH.size() < 1)
    {
        transferParams.insert("haveRecordNVH", -1.0);
        transferParams.insert("accPosNVH", 0.0);
        transferParams.insert("accOVNVH", 0.0);
        transferParams.insert("speedNVH", 0.0);
        transferParams.insert("timeNVH", 0.0);
        transferParams.insert("speedNVHError", 0.0);
        transferParams.insert("speedNVHErrorPercent", 0.0);
    }
    else
    {
        std::vector<double> tempData = recordDatasNVH.at(recordDatasNVH.size() - 1);
        transferParams.insert("haveRecordNVH", (double)recordDatasNVH.size());
        transferParams.insert("accPosNVH", tempData[0]);
        transferParams.insert("accOVNVH", tempData[1]);
        transferParams.insert("speedNVH", tempData[2]);
        transferParams.insert("timeNVH", tempData[3]);
        transferParams.insert("speedNVHError", error[2]);
        double ep = error[2] / (speed[3] - error[2]);
        if (std::isnan(ep)) ep = -340.25;
        transferParams.insert("speedNVHErrorPercent", ep);
    }

    *dataLength = 15 + 7;
    return transferParams;
}

void PedalRobotNVH::UpdateNVHLogic()
{
    if (statusNVHRun == 0)
    {
        return;
    }

    UpdateNVHLogic_NVH_DASFP();
    UpdateNVHLogic_NVH_DASCT();
    UpdateNVHLogic_NVH_CS_TestOV();
    UpdateNVHLogic_NVH_CS();
    UpdateNVHLogic_NVH_HLGS_KeepSpeed();
    UpdateNVHLogic_NVH_HLGS();
    UpdateNVHLogic_NVH_PGS();
    UpdateNVHLogic_NVH_APS();
}

void PedalRobotNVH::UpdateNVHLogic_NVH_DASFP()
{
    if (!ifRun_NVH_DASFP)
    {
        return;
    }

    if (ifOpenLoop)
    {
        switch (caseNVH) {
        case 1:
        {
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
            caseNVH = 2;
            caseChangeStopIntervalCount = 1;
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_DASFP);
            SendPedalCmd(pos);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // DAS-FP检验固定时刻下的速度，因此严格以当前时刻是否到达要求时刻为终止条件
            if (startNVHTimeInterval >= stopTime_NVH_DASFP)
            {
                caseNVH = 3;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 3:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
                SendPedalCmd(aimPos);

                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
                {
                    EndNVHLogic_NVH_DASFP();
                }
            }
            else if (caseChangeStopIntervalCount == 1)
            {
                caseChangeStopIntervalCount--;
                SendPedalCmd(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_DASFP));
                startNVHAccOV = pdUSB->GetAcceleratorOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendPedalCmd(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_DASFP));
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (caseNVH) {
        case 1:
        {
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_DASFP);
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posAim) < threshold_NVH_Loop)
            {
                caseNVH = 3;
            }
            else
            {
                caseNVH = 2;
            }
            caseChangeStopIntervalCount = 1;
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_DASFP);
            SendPedalCmd(pos);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            if (startNVHTimeInterval >= openLoopTime && fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < threshold_NVH_Loop)
            {
                caseNVH = 3;
                caseChangeStopIntervalCount = 0;
            }
            break;
        }
        case 3:
        {
            // 发送指令
            double currentOV = pdUSB->GetAcceleratorOV();
            double eignDiff = fabs(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV) - pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV + 1.0));
            SendServoCmd((aimOV_NVH_DASFP - currentOV) * pedalGain_NVH_Loop * eignDiff);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // DAS-FP检验固定时刻下的速度，因此严格以当前时刻是否到达要求时刻为终止条件
            if (startNVHTimeInterval >= stopTime_NVH_DASFP)
            {
                caseNVH = 4;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 4:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
                SendPedalCmd(aimPos);

                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
                {
                    EndNVHLogic_NVH_DASFP();
                }
            }
            else if (caseChangeStopIntervalCount == 1)
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
                startNVHAccOV = pdUSB->GetAcceleratorOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
            }
            break;
        }
        default:
            break;
        }
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_DASFP()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 获得工况误差
    double err = 0.0;
    double errp = 0.0;
    GetNVHError_NVH_DASFP(&err, &errp);

    // 保存数据
    QString workParams = "aimOV: " + QString::number(aimOV_NVH_DASFP, 'f', 1) + "\n";
    workParams += ("openLoop: " + QString(ifOpenLoop ? "True" : "False") + "\n");
    workParams += ("gain: " + QString::number(pedalGain_NVH_Loop, 'f', 2) + "\n");
    workParams += ("threshold: " + QString::number(threshold_NVH_Loop, 'f', 1) + "\n");
    workParams += ("stopTime: " + QString::number(stopTime_NVH_DASFP, 'f', 1) + "\n");
    if (stopTime_NVH_DASFP == NVH_DASFP_VERYLOWTIME)
    {
        workParams += ("expectedVelocity: " + QString::number(NVH_DASFP_VERYLOWSPEED, 'f', 1) + "\n");
    }
    else if (stopTime_NVH_DASFP == NVH_DASFP_LOWTIME)
    {
        workParams += ("expectedVelocity: " + QString::number(NVH_DASFP_LOWSPEED, 'f', 1) + "\n");
    }
    else if (stopTime_NVH_DASFP == NVH_DASFP_MODERATETIME)
    {
        workParams += ("expectedVelocity: " + QString::number(NVH_DASFP_MODERATESPEED, 'f', 1) + "\n");
    }
    workParams += ("error: " + QString::number(err, 'f', 1) + "\n");
    workParams += ("errorpercent: " + QString::number(errp*100.0, 'f', 1) + "%\n");
    SaveNVHRecordDatas("dasfp", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_DASFP = false;
    caseNVH = 1;
    statusNVHRun = 2;

    // 发送结束信号
    emit Signal_NVH_DASFP_Finished(statusNVHRun, err, errp);

    PRINTF(LOG_DEBUG, "%s: nvh-dasfp finished.\n", __func__);
}

void PedalRobotNVH::GetNVHError_NVH_DASFP(double * const err, double * const errp)
{
    int keyNum = recordDatasNVH.size() - 2;
    for (int k = keyNum; k >=0; k--)
    {
        std::vector<double> tempData = recordDatasNVH.at(k);
        if (tempData[3] <= stopTime_NVH_DASFP)
        {
            keyNum = k;
            break;
        }
    }

    std::vector<double> dataLB = recordDatasNVH.at(keyNum);
    std::vector<double> dataUB = recordDatasNVH.at(keyNum + 1);
    double timeLB = dataLB[3]; double speedLB = dataLB[2];
    double timeUB = dataUB[3]; double speedUB = dataUB[2];
    double coeff = (timeUB - stopTime_NVH_DASFP) / (timeUB - timeLB);
    double speedActual = speedLB * coeff + speedUB * (1.0 - coeff);

    if (stopTime_NVH_DASFP == NVH_DASFP_VERYLOWTIME)
    {
        *err = speedActual - NVH_DASFP_VERYLOWSPEED;
        *errp = *err / NVH_DASFP_VERYLOWSPEED;
    }
    else if (stopTime_NVH_DASFP == NVH_DASFP_LOWTIME)
    {
        *err =  speedActual - NVH_DASFP_LOWSPEED;
        *errp = *err / NVH_DASFP_LOWSPEED;
    }
    else if (stopTime_NVH_DASFP == NVH_DASFP_MODERATETIME)
    {
        *err = speedActual - NVH_DASFP_MODERATESPEED;
        *errp = *err / NVH_DASFP_MODERATESPEED;
    }
}

void PedalRobotNVH::UpdateNVHLogic_NVH_DASCT()
{
    if (!ifRun_NVH_DASCT)
    {
        return;
    }

    switch (caseNVH) {
    case 1:
    {
        SendServoCmd(0.0);
        InitialSpeedAndAcceleration(pdUSB->GetMP412CarSpeed());
        caseNVH = 2;
        caseChangeStopIntervalCount = 1;
        break;
    }
    case 2:
    {
        double speedNow = pdUSB->GetMP412CarSpeed();

        if (caseChangeStopIntervalCount <= 0)
        {
            // 更新时间
            intervalNVHTimeInterval = programmaStartTimeInterval - startNVHTime - startNVHTimeInterval;
            startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;

            // 更新反馈和误差
            int lowMaxMark = curveTemplate_NVH_DASCT.size() - 2;
            int lowMark = floor(startNVHTimeInterval / 5.0) > lowMaxMark ? lowMaxMark : floor(startNVHTimeInterval / 5.0);
            int upMark = lowMark + 1;
            double refSpeed = curveTemplate_NVH_DASCT[lowMark].second +
                    (startNVHTimeInterval - curveTemplate_NVH_DASCT[lowMark].first) /
                    (curveTemplate_NVH_DASCT[upMark].first - curveTemplate_NVH_DASCT[lowMark].first) *
                    (curveTemplate_NVH_DASCT[upMark].second - curveTemplate_NVH_DASCT[lowMark].second);
            double refAccelerationTemplate = (curveTemplate_NVH_DASCT[upMark].second - curveTemplate_NVH_DASCT[lowMark].second) /
                    (curveTemplate_NVH_DASCT[upMark].first - curveTemplate_NVH_DASCT[lowMark].first);
            deltaSpeed_NVH_DASCT = rectifyTime_NVH_DASCT * refAccelerationTemplate;

            double refAcceleration = 0.0;
            double deltaV = speedNow - refSpeed;
            if (deltaV > 0.0)
            {
                if (deltaV > deltaSpeed_NVH_DASCT)
                {
                    refAcceleration = 0.0;
                }
                else
                {
                    refAcceleration = (1 + cos(deltaV / deltaSpeed_NVH_DASCT * M_PI)) * refAccelerationTemplate / 2.0;
                }
            }
            else
            {
                if (deltaV < -deltaSpeed_NVH_DASCT)
                {
                    refAcceleration = 2.0 * refAccelerationTemplate;
                }
                else
                {
                    refAcceleration = (3 - cos(deltaV / deltaSpeed_NVH_DASCT * M_PI)) * refAccelerationTemplate / 2.0;
                }
            }

            GetSpeedAndAcceleration(refSpeed, refAcceleration);
        }
        else
        {
            // 初始化时间
            startNVHTime = programmaStartTimeInterval;
            startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            caseChangeStopIntervalCount--;

            GetSpeedAndAcceleration(curveTemplate_NVH_DASCT[0].second, (curveTemplate_NVH_DASCT[1].second - curveTemplate_NVH_DASCT[0].second) / (curveTemplate_NVH_DASCT[1].first - curveTemplate_NVH_DASCT[0].first));
        }

        // 增量控制计算
        double accP = -2.0 * accelerationGain_NVH_DASCT;
        double accI = -2.0 * accelerationGain_NVH_DASCT;
        double accD = -0.1 * accelerationGain_NVH_DASCT;
        double deltaPos = accP * (dvError[2] - dvError[1]) + accI * dvError[2] + accD * (dvError[2] - 2.0 * dvError[1] + dvError[0]);

        // 发送指令
        SendServoCmd(deltaPos);

        // 记录数据
        std::vector<double> recordUnit;
        recordUnit.clear(); recordUnit.reserve(4);
        recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
        recordUnit.push_back(pdUSB->GetAcceleratorOV());
        recordUnit.push_back(pdUSB->GetMP412CarSpeed());
        recordUnit.push_back(startNVHTimeInterval);
        recordDatasNVH.push_back(recordUnit);

        // 检验终止条件
        // DAS-CT检验固定时刻下的速度，因此严格以当前时刻是否到达要求时刻为终止条件
        if (startNVHTimeInterval >= stopTime_NVH_DASCT)
        {
            caseNVH = 3;
            caseChangeStopIntervalCount = caseChangeStopInterval;
        }
        break;
    }
    case 3:
    {
        if (caseChangeStopIntervalCount <= 0)
        {
            double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
            SendPedalCmd(aimPos);

            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
            {
                EndNVHLogic_NVH_DASCT();
            }
        }
        else if (caseChangeStopIntervalCount == 1)
        {
            caseChangeStopIntervalCount--;
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
        }
        else
        {
            caseChangeStopIntervalCount--;
            SendServoCmd(0.0);
        }
        break;
    }
    default:
        break;
    }

}

void PedalRobotNVH::EndNVHLogic_NVH_DASCT()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "acceleratedGain: " + QString::number(accelerationGain_NVH_DASCT, 'f', 2) + "\n";
    workParams += ("rectifyTime: " + QString::number(rectifyTime_NVH_DASCT, 'f', 1) + "\n");
    if (stopTime_NVH_DASCT == NVH_DASCT_VERYLOWTIME)
    {
        workParams += ("level: very low\n");
    }
    else if (stopTime_NVH_DASCT == NVH_DASCT_LOWTIME)
    {
        workParams += ("level: low\n");
    }
    else if (stopTime_NVH_DASCT == NVH_DASCT_MODERATETIME)
    {
        workParams += ("level: moderate\n");
    }
    SaveNVHRecordDatas("dasct", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_DASCT = false;
    caseNVH = 1;
    statusNVHRun = 2;

    // 发送结束信号
    emit Signal_NVH_DASCT_Finished(statusNVHRun);

    PRINTF(LOG_DEBUG, "%s: nvh-dasct finished.\n", __func__);
}

void PedalRobotNVH::UpdateNVHLogic_NVH_CS_TestOV()
{
    if (!ifRun_NVH_CS_TestOV)
    {
        return;
    }

    if (ifOpenLoop)
    {
        switch (caseNVH) {
        case 1:
        {
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
            caseNVH = 2;
            caseChangeStopIntervalCount = 1;
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_CS);
            SendPedalCmd(pos);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // CS测试开度时直接踩到所需开度，如果时间超过一定的预设时间或者已经踩到开度对应的位置则停止
            if (startNVHTimeInterval >= leastTimeInterval || fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < positionError)
            {
                caseNVH = 3;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 3:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                EndNVHLogic_NVH_CS_TestOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendPedalCmd(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_CS));
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (caseNVH) {
        case 1:
        {
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_CS);
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posAim) < threshold_NVH_Loop)
            {
                caseNVH = 3;
            }
            else
            {
                caseNVH = 2;
            }
            caseChangeStopIntervalCount = 1;
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_CS);
            SendPedalCmd(pos);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            if (startNVHTimeInterval >= openLoopTime && fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < threshold_NVH_Loop)
            {
                caseNVH = 3;
                caseChangeStopIntervalCount = 0;
            }
            break;
        }
        case 3:
        {
            // 发送指令
            double currentOV = pdUSB->GetAcceleratorOV();
            double eignDiff = fabs(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV) - pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV + 1.0));
            SendServoCmd((aimOV_NVH_CS - currentOV) * pedalGain_NVH_Loop * eignDiff);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // CS直接踩到所需开度，如果时间超过一定的预设时间则停止
            if (startNVHTimeInterval >= leastTimeInterval)
            {
                EndNVHLogic_NVH_CS_TestOV();
            }
            break;
        }
        default:
            break;
        }
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_CS_TestOV()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "aimOV: " + QString::number(aimOV_NVH_CS, 'f', 1) + "\n";
    workParams += ("openLoop: " + QString(ifOpenLoop ? "True" : "False") + "\n");
    workParams += ("gain: " + QString::number(pedalGain_NVH_Loop, 'f', 2) + "\n");
    workParams += ("threshold: " + QString::number(threshold_NVH_Loop, 'f', 1) + "\n");
    SaveNVHRecordDatas("cs_testov", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_CS_TestOV = false;
    caseNVH = 1;
    statusNVHRun = 0;

    // 发送结束信号
    emit Signal_NVH_CS_TestOVFinished();

    PRINTF(LOG_DEBUG, "%s: nvh-cs-testov finished.\n", __func__);
}

void PedalRobotNVH::UpdateNVHLogic_NVH_CS()
{
    if (!ifRun_NVH_CS)
    {
        return;
    }

    switch (caseNVH) {
    case 1:
    {
        SendServoCmd(0.0);
        initialSpeed_NVH_CS = pdUSB->GetMP412CarSpeed();
        InitialSpeedAndAcceleration(initialSpeed_NVH_CS);
        caseNVH = 2;
        caseChangeStopIntervalCount = 1;
        break;
    }
    case 2:
    {
        double speedNow = pdUSB->GetMP412CarSpeed();

        if (caseChangeStopIntervalCount <= 0)
        {
            // 更新时间
            intervalNVHTimeInterval = programmaStartTimeInterval - startNVHTime - startNVHTimeInterval;
            startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;

            // 更新反馈和误差
            double refSpeed = initialSpeed_NVH_CS + NVH_CS_ACCELERATION * startNVHTimeInterval;
            double refAcceleration = 0.0;
            double deltaV = speedNow - refSpeed;
            if (deltaV > 0.0)
            {
                if (deltaV > deltaSpeed_NVH_CS)
                {
                    refAcceleration = 0.0;
                }
                else
                {
                    refAcceleration = (1 + cos(deltaV / deltaSpeed_NVH_CS * M_PI)) * NVH_CS_ACCELERATION / 2.0;
                }
            }
            else
            {
                if (deltaV < -deltaSpeed_NVH_CS)
                {
                    refAcceleration = 2.0 * NVH_CS_ACCELERATION;
                }
                else
                {
                    refAcceleration = (3 - cos(deltaV / deltaSpeed_NVH_CS * M_PI)) * NVH_CS_ACCELERATION / 2.0;
                }
            }

            GetSpeedAndAcceleration(refSpeed, refAcceleration);
        }
        else
        {
            // 初始化时间
            startNVHTime = programmaStartTimeInterval;
            startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            caseChangeStopIntervalCount--;

            GetSpeedAndAcceleration(initialSpeed_NVH_CS, NVH_CS_ACCELERATION);
        }

        // 增量控制计算
        double accP = -2.0 * accelerationGain_NVH_CS;
        double accI = -2.0 * accelerationGain_NVH_CS;
        double accD = -0.1 * accelerationGain_NVH_CS;
        double deltaPos = accP * (dvError[2] - dvError[1]) + accI * dvError[2] + accD * (dvError[2] - 2.0 * dvError[1] + dvError[0]);

        // 发送指令
        SendServoCmd(deltaPos);

        // 记录数据
        std::vector<double> recordUnit;
        recordUnit.clear(); recordUnit.reserve(4);
        recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
        recordUnit.push_back(pdUSB->GetAcceleratorOV());
        recordUnit.push_back(pdUSB->GetMP412CarSpeed());
        recordUnit.push_back(startNVHTimeInterval);
        recordDatasNVH.push_back(recordUnit);

        // 检验终止条件
        // CS工况速度达到目标值或者使用时间超时则停止，以速度为主，时间为异常处理
        if (speedNow >= NVH_CS_TERMINATEDSPEED || startNVHTimeInterval >= (NVH_CS_TERMINATEDSPEED - initialSpeed_NVH_CS) / NVH_CS_ACCELERATION + overTimeInterval)
        {
            caseNVH = 3;
            caseChangeStopIntervalCount = caseChangeStopInterval;
        }
        break;
    }
    case 3:
    {
        if (caseChangeStopIntervalCount <= 0)
        {
            double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
            SendPedalCmd(aimPos);

            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
            {
                EndNVHLogic_NVH_CS();
            }
        }
        else if (caseChangeStopIntervalCount == 1)
        {
            caseChangeStopIntervalCount--;
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
        }
        else
        {
            caseChangeStopIntervalCount--;
            SendServoCmd(0.0);
        }
        break;
    }
    default:
        break;
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_CS()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "acceleratedGain: " + QString::number(accelerationGain_NVH_CS, 'f', 2) + "\n";
    workParams += ("deltaSpeed: " + QString::number(deltaSpeed_NVH_CS, 'f', 1) + "\n");
    SaveNVHRecordDatas("cs", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_CS = false;
    caseNVH = 1;
    statusNVHRun = 2;

    // 发送结束信号
    emit Signal_NVH_CS_Finished(statusNVHRun);

    PRINTF(LOG_DEBUG, "%s: nvh-cs finished.\n", __func__);
}

void PedalRobotNVH::UpdateNVHLogic_NVH_HLGS_KeepSpeed()
{
    if (!ifRun_NVH_HLGS_KeepSpeed)
    {
        return;
    }

    switch (caseNVH) {
    case 1:
    {
        SendServoCmd(0.0);
        InitialSpeedAndAcceleration(pdUSB->GetMP412CarSpeed());
        accelerated_NVH_HLGS = aimSpeed_NVH_HLGS > pdUSB->GetMP412CarSpeed();
        caseNVH = 2;
        caseChangeStopIntervalCount = 1;
        break;
    }
    case 2:
    {
        double speedNow = pdUSB->GetMP412CarSpeed();

        if (caseChangeStopIntervalCount <= 0)
        {
            // 更新时间
            intervalNVHTimeInterval = programmaStartTimeInterval - startNVHTime - startNVHTimeInterval;
            startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;

            // 更新反馈和误差
            if (accelerated_NVH_HLGS) // 加速
            {
                if (speedNow < aimSpeed_NVH_HLGS - speedDifference_NVH_HLGS_AccelerationChanging)
                {
                    GetSpeedAndAcceleration(aimSpeed_NVH_HLGS, acceleration_NVH_HLGS);
                }
                else
                {
                    double coeff = 1.0 - (speedNow - aimSpeed_NVH_HLGS + speedDifference_NVH_HLGS_AccelerationChanging) / speedDifference_NVH_HLGS_AccelerationChanging;
                    coeff = coeff > 1.0 ? 1.0 : coeff;
                    coeff = coeff < -1.0 ? -1.0 : coeff;
                    GetSpeedAndAcceleration(aimSpeed_NVH_HLGS, acceleration_NVH_HLGS * coeff);
                }
            }
            else // 减速
            {
                if (speedNow > aimSpeed_NVH_HLGS + speedDifference_NVH_HLGS_AccelerationChanging)
                {
                    GetSpeedAndAcceleration(aimSpeed_NVH_HLGS, -acceleration_NVH_HLGS);
                }
                else
                {
                    double coeff = 1.0 - (aimSpeed_NVH_HLGS + speedDifference_NVH_HLGS_AccelerationChanging - speedNow) / speedDifference_NVH_HLGS_AccelerationChanging;
                    coeff = coeff > 1.0 ? 1.0 : coeff;
                    coeff = coeff < -1.0 ? -1.0 : coeff;
                    GetSpeedAndAcceleration(aimSpeed_NVH_HLGS, -acceleration_NVH_HLGS * coeff);
                }
            }
        }
        else
        {
            // 初始化时间
            startNVHTime = programmaStartTimeInterval;
            startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            caseChangeStopIntervalCount--;
        }

        // 增量控制计算
        double accP = -2.0 * accelerationGain_NVH_HLGS;
        double accI = -2.0 * accelerationGain_NVH_HLGS;
        double accD = -0.1 * accelerationGain_NVH_HLGS;
        double deltaPos = accP * (dvError[2] - dvError[1]) + accI * dvError[2] + accD * (dvError[2] - 2.0 * dvError[1] + dvError[0]);

        // 发送指令
        SendServoCmd(deltaPos);

        // 记录数据
        std::vector<double> recordUnit;
        recordUnit.clear(); recordUnit.reserve(4);
        recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
        recordUnit.push_back(pdUSB->GetAcceleratorOV());
        recordUnit.push_back(pdUSB->GetMP412CarSpeed());
        recordUnit.push_back(startNVHTimeInterval);
        recordDatasNVH.push_back(recordUnit);

        // 检验终止条件
        // HLGS在保持速度上如果时间超过预设等待时间则不再进行踏板控制
        if (startNVHTimeInterval >= waitTimeInterval)
        {
            caseNVH = 3;
        }
        break;
    }
    case 3:
    {
        EndNVHLogic_NVH_HLGS_KeepSpeed();
        break;
    }
    default:
        break;
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_HLGS_KeepSpeed()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "aimSpeed: " + QString::number(aimSpeed_NVH_HLGS, 'f', 1) + "\n";
    workParams += ("risingAcceleration: " + QString::number(acceleration_NVH_HLGS, 'f', 1) + "\n");
    workParams += ("thresholdSpeedDifference: " + QString::number(speedDifference_NVH_HLGS_AccelerationChanging, 'f', 1) + "\n");
    workParams += ("acceleratedGain: " + QString::number(accelerationGain_NVH_HLGS, 'f', 2) + "\n");
    SaveNVHRecordDatas("hlgs_keepspeed", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_HLGS_KeepSpeed = false;
    caseNVH = 1;
    statusNVHRun = 0;

    // 发送结束信号
    emit Signal_NVH_HLGS_KeepSpeedFinished();

    PRINTF(LOG_DEBUG, "%s: nvh-hlgs-keepspeed finished.\n", __func__);
}

void PedalRobotNVH::UpdateNVHLogic_NVH_HLGS()
{
    if (!ifRun_NVH_HLGS)
    {
        return;
    }

    if (ifOpenLoop)
    {
        switch (caseNVH) {
        case 1:
        {
            // 发送指令
            double pos = Configuration::Instance()->limitNeg[1] + 2.0;
            SendPedalCmd(pos);

            // 检验终止条件
            // 先回到起点
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < positionError)
            {
                caseNVH = 2;
            }
            break;
        }
        case 2:
        {
            // 发送指令
            double posStart = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_HLGS);
            SendPedalCmd(posStart);

            // 检验终止条件
            // 到达初始开度，以开度对应的位置为终止条件
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posStart) < positionError)
            {
                startNVHAccOV = pdUSB->GetAcceleratorOV();
                caseNVH = 3;
                caseChangeStopIntervalCount = 1;
            }
            break;
        }
        case 3:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 发送指令
            double posStart = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_HLGS);
            double posEnd = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimUpOV_NVH_HLGS);
            double timeNow = startNVHTimeInterval > usingTime_NVH_HLGS ? usingTime_NVH_HLGS : startNVHTimeInterval;
            double aimOV = timeNow / usingTime_NVH_HLGS * (aimUpOV_NVH_HLGS - aimLowOV_NVH_HLGS) + aimLowOV_NVH_HLGS;
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV);
            posAim = posAim < posStart ? posStart : posAim;
            posAim = posAim > posEnd ? posEnd : posAim;
            SendPedalCmd(posAim + advancedPos_NVH_HLGS);

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // HLGS以开度对应的位置为主要终止条件，以所需时间为异常处理
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posEnd) < positionError || startNVHTimeInterval >= usingTime_NVH_HLGS + overTimeInterval)
            {
                caseNVH = 4;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 4:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
                SendPedalCmd(aimPos);

                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
                {
                    EndNVHLogic_NVH_HLGS();
                }
            }
            else if (caseChangeStopIntervalCount == 1)
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
                startNVHAccOV = pdUSB->GetAcceleratorOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (caseNVH) {
        case 1:
        {
            // 发送指令
            double pos = Configuration::Instance()->limitNeg[1] + 2.0;
            SendPedalCmd(pos);

            // 检验终止条件
            // 先回到起点
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < positionError)
            {
                double posAim = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_HLGS);
                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posAim) < threshold_NVH_Loop)
                {
                    caseNVH = 3;
                }
                else
                {
                    caseNVH = 2;
                }
            }
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_HLGS);
            SendPedalCmd(pos);

            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < threshold_NVH_Loop)
            {
                caseNVH = 3;
            }
            break;
        }
        case 3:
        {
            // 发送指令
            double currentOV = pdUSB->GetAcceleratorOV();
            double eignDiff = fabs(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV) - pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV + 1.0));
            SendServoCmd((aimLowOV_NVH_HLGS - currentOV) * pedalGain_NVH_Loop * eignDiff);

            // 检验终止条件
            // 先踩到所需下极限开度，如果时间超过一定的预设时间则停止
            if (startNVHTimeInterval >= leastTimeInterval || fabs(pdUSB->GetAcceleratorOV() - aimLowOV_NVH_HLGS) < openValueError)
            {
                startNVHAccOV = pdUSB->GetAcceleratorOV();
                caseNVH = 4;
                caseChangeStopIntervalCount = 1;
            }
            break;
        }
        case 4:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 发送指令
            double posStart = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_HLGS);
            double posEnd = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimUpOV_NVH_HLGS);
            double timeNow = startNVHTimeInterval > usingTime_NVH_HLGS ? usingTime_NVH_HLGS : startNVHTimeInterval;
            double aimOV = timeNow / usingTime_NVH_HLGS * (aimUpOV_NVH_HLGS - aimLowOV_NVH_HLGS) + aimLowOV_NVH_HLGS;
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV);
            posAim = posAim < posStart ? posStart : posAim;
            posAim = posAim > posEnd ? posEnd : posAim;
            SendPedalCmd(posAim + advancedPos_NVH_HLGS);

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // HLGS以时间为终止指标
            if (startNVHTimeInterval >= usingTime_NVH_HLGS || fabs(pdUSB->GetAcceleratorOV() - aimUpOV_NVH_HLGS) < openValueError)
            {
                caseNVH = 5;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 5:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
                SendPedalCmd(aimPos);

                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
                {
                    EndNVHLogic_NVH_HLGS();
                }
            }
            else if (caseChangeStopIntervalCount == 1)
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
                startNVHAccOV = pdUSB->GetAcceleratorOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
            }
            break;
        }
        default:
            break;
        }
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_HLGS()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "aimLowOV: " + QString::number(aimLowOV_NVH_HLGS, 'f', 1) + "\n";
    workParams += "aimUpOV: " + QString::number(aimUpOV_NVH_HLGS, 'f', 1) + "\n";
    workParams += ("openLoop: " + QString(ifOpenLoop ? "True" : "False") + "\n");
    workParams += ("gain: " + QString::number(pedalGain_NVH_Loop, 'f', 2) + "\n");
    workParams += ("threshold: " + QString::number(threshold_NVH_Loop, 'f', 1) + "\n");
    workParams += ("compensate: " + QString::number(advancedPos_NVH_HLGS, 'f', 1) + "\n");
    SaveNVHRecordDatas("hlgs", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_HLGS = false;
    caseNVH = 1;
    statusNVHRun = 2;

    // 发送结束信号
    emit Signal_NVH_HLGS_Finished(statusNVHRun);

    PRINTF(LOG_DEBUG, "%s: nvh-hlgs finished.\n", __func__);
}

void PedalRobotNVH::UpdateNVHLogic_NVH_PGS()
{
    if (!ifRun_NVH_PGS)
    {
        return;
    }

    if (ifOpenLoop)
    {
        switch (caseNVH) {
        case 1:
        {
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
            caseNVH = 2;
            caseChangeStopIntervalCount = 1;
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_PGS);
            SendPedalCmd(pos);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // PGS直接踩到所需开度，如果时间超过一定的预设时间或者已经踩到开度对应的位置则停止
            if (startNVHTimeInterval >= leastTimeInterval || fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < positionError)
            {
                caseNVH = 3;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 3:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                EndNVHLogic_NVH_PGS();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendPedalCmd(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_PGS));
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (caseNVH) {
        case 1:
        {
            SendServoCmd(0.0);
            startNVHAccOV = pdUSB->GetAcceleratorOV();
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_PGS);
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posAim) < threshold_NVH_Loop)
            {
                caseNVH = 3;
            }
            else
            {
                caseNVH = 2;
            }
            caseChangeStopIntervalCount = 1;
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV_NVH_PGS);
            SendPedalCmd(pos);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            if (startNVHTimeInterval >= openLoopTime && fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < threshold_NVH_Loop)
            {
                caseNVH = 3;
                caseChangeStopIntervalCount = 0;
            }
            break;
        }
        case 3:
        {
            // 发送指令
            double currentOV = pdUSB->GetAcceleratorOV();
            double eignDiff = fabs(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV) - pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV + 1.0));
            SendServoCmd((aimOV_NVH_PGS - currentOV) * pedalGain_NVH_Loop * eignDiff);

            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // PGS直接踩到所需开度，如果时间超过一定的预设时间则停止
            if (startNVHTimeInterval >= leastTimeInterval)
            {
                EndNVHLogic_NVH_PGS();
            }
            break;
        }
        default:
            break;
        }
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_PGS()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "aimOV: " + QString::number(aimOV_NVH_PGS, 'f', 1) + "\n";
    workParams += ("openLoop: " + QString(ifOpenLoop ? "True" : "False") + "\n");
    workParams += ("gain: " + QString::number(pedalGain_NVH_Loop, 'f', 2) + "\n");
    workParams += ("threshold: " + QString::number(threshold_NVH_Loop, 'f', 1) + "\n");
    SaveNVHRecordDatas("pgs", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_PGS = false;
    caseNVH = 1;
    statusNVHRun = 2;

    // 发送结束信号
    emit Signal_NVH_PGS_Finished(statusNVHRun);

    PRINTF(LOG_DEBUG, "%s: nvh-pgs finished.\n", __func__);
}

void PedalRobotNVH::UpdateNVHLogic_NVH_APS()
{
    if (!ifRun_NVH_APS)
    {
        return;
    }

    if (ifOpenLoop)
    {
        switch (caseNVH) {
        case 1:
        {
            // 发送指令
            double pos = Configuration::Instance()->limitNeg[1] + 2.0;
            SendPedalCmd(pos);

            // 检验终止条件
            // 先回到起点
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < positionError)
            {
                caseNVH = 2;
            }
            break;
        }
        case 2:
        {
            // 发送指令
            double posStart = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_APS);
            SendPedalCmd(posStart);

            // 检验终止条件
            // 到达初始开度对应的位置
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posStart) < positionError)
            {
                startNVHAccOV = pdUSB->GetAcceleratorOV();
                caseNVH = 3;
                caseChangeStopIntervalCount = 1;
            }
            break;
        }
        case 3:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 发送指令
            double posStart = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_APS);
            double posEnd = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimUpOV_NVH_APS);
            double timeNow = startNVHTimeInterval > usingTime_NVH_APS ? usingTime_NVH_APS : startNVHTimeInterval;
            double aimOV = timeNow / usingTime_NVH_APS * (aimUpOV_NVH_APS - aimLowOV_NVH_APS) + aimLowOV_NVH_APS;
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV);
            posAim = posAim < posStart ? posStart : posAim;
            posAim = posAim > posEnd ? posEnd : posAim;
            SendPedalCmd(posAim + advancedPos_NVH_APS);

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // APS以开度对应的位置为主要终止条件，以所需时间为异常处理
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posEnd) < positionError || startNVHTimeInterval >= usingTime_NVH_APS + overTimeInterval)
            {
                caseNVH = 4;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 4:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
                SendPedalCmd(aimPos);

                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
                {
                    EndNVHLogic_NVH_APS();
                }
            }
            else if (caseChangeStopIntervalCount == 1)
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
                startNVHAccOV = pdUSB->GetAcceleratorOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
            }
            break;
        }
        default:
            break;
        }
    }
    else
    {
        switch (caseNVH) {
        case 1:
        {
            // 发送指令
            double pos = Configuration::Instance()->limitNeg[1] + 2.0;
            SendPedalCmd(pos);

            // 检验终止条件
            // 先回到起点
            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < positionError)
            {
                double posAim = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_APS);
                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - posAim) < threshold_NVH_Loop)
                {
                    caseNVH = 3;
                }
                else
                {
                    caseNVH = 2;
                }
            }
            break;
        }
        case 2:
        {
            // 发送指令
            double pos = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_APS);
            SendPedalCmd(pos);

            if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - pos) < threshold_NVH_Loop)
            {
                caseNVH = 3;
            }
            break;
        }
        case 3:
        {
            // 发送指令
            double currentOV = pdUSB->GetAcceleratorOV();
            double eignDiff = fabs(pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV) - pdPreTest->GetPositionFromOpenValue(startNVHAccOV, currentOV + 1.0));
            SendServoCmd((aimLowOV_NVH_APS - currentOV) * pedalGain_NVH_Loop * eignDiff);

            // 检验终止条件
            // 先踩到所需下极限开度，如果时间超过一定的预设时间则停止
            if (startNVHTimeInterval >= leastTimeInterval || fabs(pdUSB->GetAcceleratorOV() - aimLowOV_NVH_APS) < openValueError)
            {
                startNVHAccOV = pdUSB->GetAcceleratorOV();
                caseNVH = 4;
                caseChangeStopIntervalCount = 1;
            }
            break;
        }
        case 4:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                // 更新时间
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
            }
            else
            {
                // 初始化时间
                startNVHTime = programmaStartTimeInterval;
                startNVHTimeInterval = programmaStartTimeInterval - startNVHTime;
                caseChangeStopIntervalCount--;
            }

            // 发送指令
            double posStart = pdPreTest->GetPositionFromOpenValue(-1.0, aimLowOV_NVH_APS);
            double posEnd = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimUpOV_NVH_APS);
            double timeNow = startNVHTimeInterval > usingTime_NVH_APS ? usingTime_NVH_APS : startNVHTimeInterval;
            double aimOV = timeNow / usingTime_NVH_APS * (aimUpOV_NVH_APS - aimLowOV_NVH_APS) + aimLowOV_NVH_APS;
            double posAim = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, aimOV);
            posAim = posAim < posStart ? posStart : posAim;
            posAim = posAim > posEnd ? posEnd : posAim;
            SendPedalCmd(posAim + advancedPos_NVH_APS);

            // 记录数据
            std::vector<double> recordUnit;
            recordUnit.clear(); recordUnit.reserve(4);
            recordUnit.push_back(RobotThetaMatrix::Instance()->GetTheta(1));
            recordUnit.push_back(pdUSB->GetAcceleratorOV());
            recordUnit.push_back(pdUSB->GetMP412CarSpeed());
            recordUnit.push_back(startNVHTimeInterval);
            recordDatasNVH.push_back(recordUnit);

            // 检验终止条件
            // APS以时间为终止指标
            if (startNVHTimeInterval >= usingTime_NVH_APS || fabs(pdUSB->GetAcceleratorOV() - aimUpOV_NVH_APS) < openValueError)
            {
                caseNVH = 5;
                caseChangeStopIntervalCount = caseChangeStopInterval;
            }
            break;
        }
        case 5:
        {
            if (caseChangeStopIntervalCount <= 0)
            {
                double aimPos = pdPreTest->GetPositionFromOpenValue(startNVHAccOV, pdPreTest->GetLimitOpenValue(1, false));
                SendPedalCmd(aimPos);

                if (fabs(RobotThetaMatrix::Instance()->GetTheta(1) - aimPos) < positionError)
                {
                    EndNVHLogic_NVH_APS();
                }
            }
            else if (caseChangeStopIntervalCount == 1)
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
                startNVHAccOV = pdUSB->GetAcceleratorOV();
            }
            else
            {
                caseChangeStopIntervalCount--;
                SendServoCmd(0.0);
            }
            break;
        }
        default:
            break;
        }
    }
}

void PedalRobotNVH::EndNVHLogic_NVH_APS()
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 保存数据
    QString workParams = "aimLowOV: " + QString::number(aimLowOV_NVH_APS, 'f', 1) + "\n";
    workParams += "aimUpOV: " + QString::number(aimUpOV_NVH_APS, 'f', 1) + "\n";
    workParams += ("openLoop: " + QString(ifOpenLoop ? "True" : "False") + "\n");
    workParams += ("gain: " + QString::number(pedalGain_NVH_Loop, 'f', 2) + "\n");
    workParams += ("threshold: " + QString::number(threshold_NVH_Loop, 'f', 1) + "\n");
    workParams += ("compensate: " + QString::number(advancedPos_NVH_APS, 'f', 1) + "\n");
    SaveNVHRecordDatas("aps", workParams);

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    ifRun_NVH_APS = false;
    caseNVH = 1;
    statusNVHRun = 2;

    // 发送结束信号
    emit Signal_NVH_APS_Finished(statusNVHRun);

    PRINTF(LOG_DEBUG, "%s: nvh-aps finished.\n", __func__);
}

void PedalRobotNVH::SaveNVHRecordDatas(QString workMode, QString workParams)
{
    QString transferFileName = QDateTime::currentDateTime().date().toString("yyyy-MM-dd") + " " +
            QString::number(QTime::currentTime().hour()) + "-" +
            QString::number(QTime::currentTime().minute()) + "-" +
            QString::number(QTime::currentTime().second());

   std::string filePath = Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + recordNVHDataDirectory + (transferFileName + " nvh_" + workMode + ".txt").toStdString();
   std::string fileContent = "WorkParams\n" + workParams.toStdString() + "AccPos/mm AccOV/% Speed/kmh Time/s Others...\n";

   for (size_t h = 0; h < recordDatasNVH.size(); h++)
   {
       std::vector<double> tempData = recordDatasNVH.at(h);
       fileContent += QString::number(tempData[0], 'f', 2).toStdString() + " " +
               QString::number(tempData[1], 'f', 2).toStdString() + " " +
               QString::number(tempData[2], 'f', 2).toStdString() + " " +
               QString::number(tempData[3], 'f', 3).toStdString();
       if (tempData.size() <= 4) fileContent += "\n";
       else
       {
           for (size_t g = 4; g < tempData.size(); g++)
           {
               fileContent += " " + QString::number(tempData[g], 'f', 3).toStdString();
           }
           fileContent += "\n";
       }
   }

   NormalFile::WriteToFile(filePath.c_str(), fileContent.c_str(), fileContent.size());
   PRINTF(LOG_DEBUG, "%s: %s saved.\n", __func__, filePath.c_str());
}

void PedalRobotNVH::ImmediateEndNVHCtrl(bool ifLiftPedal)
{
    if (statusNVHRun != 0) statusNVHRun = 0;
    if (ifRun_NVH_DASFP)
    {
        ifRun_NVH_DASFP = false;

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        if (ifLiftPedal)
        {
            // 回抬踏板
            bool stopPoss = ResetSoftStopFile();
            if (!stopPoss)
            {
                return;
            }

            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送结束信号
        emit Signal_NVH_DASFP_Finished(statusNVHRun);

        PRINTF(LOG_NOTICE, "%s: nvh-dasfp closed by user.\n", __func__);
    }
    if (ifRun_NVH_DASCT)
    {
        ifRun_NVH_DASCT = false;

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        if (ifLiftPedal)
        {
            // 回抬踏板
            bool stopPoss = ResetSoftStopFile();
            if (!stopPoss)
            {
                return;
            }

            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送结束信号
        emit Signal_NVH_DASCT_Finished(statusNVHRun);

        PRINTF(LOG_NOTICE, "%s: nvh-dasct closed by user.\n", __func__);
    }
    if (ifRun_NVH_CS)
    {
        ifRun_NVH_CS = false;

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        if (ifLiftPedal)
        {
            // 回抬踏板
            bool stopPoss = ResetSoftStopFile();
            if (!stopPoss)
            {
                return;
            }

            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送结束信号
        emit Signal_NVH_CS_Finished(statusNVHRun);

        PRINTF(LOG_NOTICE, "%s: nvh-cs closed by user.\n", __func__);
    }
    if (ifRun_NVH_HLGS)
    {
        ifRun_NVH_HLGS = false;

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        if (ifLiftPedal)
        {
            // 回抬踏板
            bool stopPoss = ResetSoftStopFile();
            if (!stopPoss)
            {
                return;
            }

            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送结束信号
        emit Signal_NVH_HLGS_Finished(statusNVHRun);

        PRINTF(LOG_NOTICE, "%s: nvh-hlgs closed by user.\n", __func__);
    }
    if (ifRun_NVH_PGS)
    {
        ifRun_NVH_PGS = false;

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        if (ifLiftPedal)
        {
            // 回抬踏板
            bool stopPoss = ResetSoftStopFile();
            if (!stopPoss)
            {
                return;
            }

            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送结束信号
        emit Signal_NVH_PGS_Finished(statusNVHRun);

        PRINTF(LOG_NOTICE, "%s: nvh-pgs closed by user.\n", __func__);
    }
    if (ifRun_NVH_APS)
    {
        ifRun_NVH_APS = false;

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        if (ifLiftPedal)
        {
            // 回抬踏板
            bool stopPoss = ResetSoftStopFile();
            if (!stopPoss)
            {
                return;
            }

            std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
            UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
        }

        // 发送结束信号
        emit Signal_NVH_APS_Finished(statusNVHRun);

        PRINTF(LOG_NOTICE, "%s: nvh-aps closed by user.\n", __func__);
    }
}

void PedalRobotNVH::NotImmediateEndNVHCtrl()
{
    if (ifRun_NVH_CS_TestOV)
    {
        ifRun_NVH_CS_TestOV = false;

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        // 保存数据
        SaveNVHRecordDatas("cs_testov");

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

         // 发送结束信号
        emit Signal_NVH_CS_TestOVFinished();

        PRINTF(LOG_DEBUG, "%s: nvh-cs-testov closed by user.\n", __func__);
    }
    if (ifRun_NVH_HLGS_KeepSpeed)
    {
        ifRun_NVH_HLGS_KeepSpeed = false;

        // 停止
        UnifiedRobot::Instance()->SendEnterIdleMsg();

        // 保存数据
        SaveNVHRecordDatas("hlgs_keepspeed");

        // 初始化部分参数
        recordDatasNVH.clear();
        recordDatasNVH.reserve(2000);

         // 发送结束信号
        emit Signal_NVH_HLGS_KeepSpeedFinished();

        PRINTF(LOG_DEBUG, "%s: nvh-hlgs-keepspeed closed by user.\n", __func__);
    }
}

void PedalRobotNVH::SendPedalCmd(double accPos)
{
    std::vector<int> actionAxes(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionAxes[i] = i;
    }

    std::vector<int> actionMethod(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionMethod[i] = MsgStructure::MonitorCommand::AbsControl;
    }

    std::vector<double> actionTheta(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionTheta[i] = 0.0;
    }
    actionTheta[1] = accPos;

    UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
}

void PedalRobotNVH::SendServoCmd(double accPos)
{
    std::vector<int> actionAxes(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionAxes[i] = i;
    }

    std::vector<int> actionMethod(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionMethod[i] = MsgStructure::MonitorCommand::DeltaControl;
    }

    std::vector<double> actionTheta(RobotParams::axisNum);
    for(int i=0; i<RobotParams::axisNum; ++i){
        actionTheta[i] = 0.0;
    }
    actionTheta[0] = -100.0;
    actionTheta[1] = accPos;

    UnifiedRobot::Instance()->SendMonitorCommandMsg(actionMethod, actionAxes, actionTheta);
}

void PedalRobotNVH::InitialSpeedAndAcceleration(double initialSpeed, double initialAcceleration)
{
    int initialSpeedInter = (int)(initialSpeed * 100.0); // 取小数点后2位
    double initialSpeedNow = ((double)initialSpeedInter) / 100.0;

    speed[0] = initialSpeedNow; dv[0] = initialAcceleration;
    speed[1] = initialSpeedNow; dv[1] = initialAcceleration;
    speed[2] = initialSpeedNow; dv[2] = initialAcceleration;
    speed[3] = initialSpeedNow;

    error[0] = 0.0; dvError[0] = 0.0;
    error[1] = 0.0; dvError[1] = 0.0;
    error[2] = 0.0; dvError[2] = 0.0;
}

void PedalRobotNVH::GetSpeedAndAcceleration(double refSpeed, double refAcceleration)
{
    speed[0] = speed[1];
    speed[1] = speed[2];
    speed[2] = speed[3];
    int speedInter = (int)(pdUSB->GetMP412CarSpeed() * 100.0); // 取小数点后2位
    double speedNow = ((double)speedInter) / 100.0;
    speed[3] = speedNow;
    speed[3] = pdUSB->GetMP412CarSpeed();

    dv[0] = dv[1];
    dv[1] = dv[2];
    dv[2] = dvAlpha * dv[2] + (1 - dvAlpha) * (speed[3] - speed[2]) / intervalNVHTimeInterval;

    error[0] = error[1];
    error[1] = error[2];
    error[2] = speed[3] - refSpeed;

    dvError[0] = dvError[1];
    dvError[1] = dvError[2];
    dvError[2] = dv[2] - refAcceleration;
}

void PedalRobotNVH::RefreshControlMode()
{
    ifOpenLoop = Configuration::Instance()->sysControlParams[1] < 0.5 ? true : false;
    pedalGain_NVH_Loop = Configuration::Instance()->sysControlParams[2];
    threshold_NVH_Loop = Configuration::Instance()->sysControlParams[3];
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
void PedalRobotNVH::Slot_SingleAxisMove(int axisNum, int direction)
{
    if (!pdPreTest->ifEnableSingleMoveDuringPreTest)
    {
        StopAllModuleMotion();
    }

    const int axisIndex = axisNum;
    if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: axisIndex(%d) out of range.\n", __func__, axisIndex);
        return;
    }

    double speed = direction * Configuration::Instance()->singleAxisBtnRatio[axisIndex];
    //不同的丝杆导程 需要处理下(以10mm导程为基准)
    speed *= Configuration::Instance()->machineParam[5];//单轴运动的速度倍率

    UnifiedRobot::Instance()->SendSingleAxisMsg(axisIndex, speed);
}

void PedalRobotNVH::Slot_EnterIdle()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
}

void PedalRobotNVH::Slot_GoHome()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
    StopAllModuleMotion();

    bool goHomePoss = ResetOriginFileByDeathValue();
    if (!goHomePoss)
    {
        return;
    }

    UnifiedRobot::Instance()->SendGoHomeMsg();
}

void PedalRobotNVH::Slot_SoftStop()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
    StopAllModuleMotion();
}

void PedalRobotNVH::Slot_SoftStopAndLift()
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
    StopAllModuleMotion();

    bool stopPoss = ResetSoftStopFile();
    if (!stopPoss)
    {
        return;
    }

    std::string actionFilePath = Configuration::Instance()->GetFilePath(Configuration::RobotSoftStopFilePath);
    UnifiedRobot::Instance()->StartActionByTeachFile(actionFilePath);
}

void PedalRobotNVH::Slot_BeginPreTest(
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
        int delayCircuit)
{
    UnifiedRobot::Instance()->SendEnterIdleMsg();
    pdPreTest->BeginPreTest(
                positionInterval,
                backDistance,
                ifAutoBrk,
                NotAutoBrkLimit,
                ifAutoAcc,
                NotAutoAccLimit,
                ifO2V,
                o2VNum,
                o2VStopTime,
                o2VStopVelocity,
                ifDecelerateByBrk,
                ifTransfer,
                wantedAxisNum,
                ifBothAxis,
                delayCircuit);
}

void PedalRobotNVH::Slot_StopPreTestRightNow()
{
    pdPreTest->ImmediateEndPreTest();
}

void PedalRobotNVH::Slot_ReadPreTestFile(QString pickFilePath, bool ifStraightRead)
{
    if (!ifStraightRead)
    {
        pdPreTest->PickTransferFileFromAnotherPath(pickFilePath);
    }
    pdPreTest->ReadSavedPreTestFile();
}

void PedalRobotNVH::Slot_ContinueO2VTest()
{
    pdPreTest->o2VTestPauseForSignal = false;
}

void PedalRobotNVH::Slot_LogicUpdateTimeOut(int currentTimeoutCount, int intervalMultiplier)
{
    //踏板机器人设置的intervalMultiplier=3
    int currentCount = currentTimeoutCount % intervalMultiplier;
    switch(currentCount){
    case 0:
        //即18ms*3的控制周期 进行机器人的控制
        UpdateLogicControl();
        break;
    case 1:
        pdUSB->UpdateDeviceReady();
        pUri->UpdateLogic();
        break;
    case 2:
        pdUSB->UpdateDeviceControl();
        break;
    default:
        PRINTF(LOG_ERR, "%s: invalid currentCount=[%d]\n", __func__, currentCount);
        return;
    }
}

void PedalRobotNVH::Slot_WidgetUpdateTimeOut()
{
    bool isGoHomed = UnifiedRobot::Instance()->GetIsGoHomed();
    static bool isGoHomedLast = !isGoHomed;//初始值 保证进入if判断
    if(isGoHomed != isGoHomedLast){//回原的状态有变化
        isGoHomedLast = isGoHomed;
        goHomeState = isGoHomed ? 1 : 0;
    }
    else
    {
        goHomeState = -1;
    }

    int dataLength = 0;
    paraMap transferParameters = UpdateTransferParams(&dataLength);
    emit Signal_UpdateWidget(transferParameters, dataLength);
}

void PedalRobotNVH::Slot_PreTestFinished()
{
    emit Signal_UpdateWidgetForPreTestFinished();
}

void PedalRobotNVH::Slot_O2VWaitBegin(bool ifStartWait)
{
    emit Signal_UpdateWidgetForO2VWaitBegin(ifStartWait);
}

void PedalRobotNVH::Slot_NVH_DASFP_GetRecommandOV(int level)
{
    double wantedOV = 0.0;
    switch (level)
    {
    case 0:
//        wantedOV = pdPreTest->GetOVFromTimeAndSpeed(NVH_DASFP_VERYLOWTIME, NVH_DASFP_VERYLOWSPEED);
        wantedOV = 10.0;
        break;
    case 1:
//        wantedOV = pdPreTest->GetOVFromTimeAndSpeed(NVH_DASFP_LOWTIME, NVH_DASFP_LOWSPEED);
        wantedOV = 23.0;
        break;
    case 2:
//        wantedOV = pdPreTest->GetOVFromTimeAndSpeed(NVH_DASFP_MODERATETIME, NVH_DASFP_MODERATESPEED);
        wantedOV = 40.0;
        break;
    default:
        break;
    }

    emit Signal_NVH_DASFP_UpdateWidgetForRecommandOV(wantedOV);
}

void PedalRobotNVH::Slot_NVH_DASFP_Start(double aimOV, int level)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 计算所需开度和停止时间
    aimOV_NVH_DASFP = aimOV;
    switch (level)
    {
    case 0:
        stopTime_NVH_DASFP = NVH_DASFP_VERYLOWTIME;
        break;
    case 1:
        stopTime_NVH_DASFP = NVH_DASFP_LOWTIME;
        break;
    case 2:
        stopTime_NVH_DASFP = NVH_DASFP_MODERATETIME;
        break;
    default:
        break;
    }

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_DASFP = true;
}

void PedalRobotNVH::Slot_NVH_DASCT_Start(double accGain, double rectifyTime, int level)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 设定控制参数
    accelerationGain_NVH_DASCT = accGain;
    rectifyTime_NVH_DASCT = rectifyTime;

    // 计算停止时间
    switch (level)
    {
    case 0:
        stopTime_NVH_DASCT = NVH_DASCT_VERYLOWTIME;
        curveTemplate_NVH_DASCT = NVH_DASCT_VERYLOWCURVE;
        break;
    case 1:
        stopTime_NVH_DASCT = NVH_DASCT_LOWTIME;
        curveTemplate_NVH_DASCT = NVH_DASCT_LOWCURVE;
        break;
    case 2:
        stopTime_NVH_DASCT = NVH_DASCT_MODERATETIME;
        curveTemplate_NVH_DASCT = NVH_DASCT_MODERATECURVE;
        break;
    default:
        break;
    }

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_DASCT = true;
}

void PedalRobotNVH::Slot_NVH_CS_TestOV(double aimOV)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 计算所需开度和停止时间
    aimOV_NVH_CS = aimOV;

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_CS_TestOV = true;
}

void PedalRobotNVH::Slot_NVH_CS_Start(double accGain, double rectifyTime)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

     // 设定控制参数
    accelerationGain_NVH_CS = accGain;
    rectifyTime_NVH_CS = rectifyTime;
    deltaSpeed_NVH_CS = rectifyTime_NVH_CS * NVH_CS_ACCELERATION;

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_CS = true;
}

void PedalRobotNVH::Slot_NVH_HLGS_KeepSpeed(double aimSpeed, double aimAcceleration,
                                            double approachTime, double accelerationGain)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 获得目标车速
    aimSpeed_NVH_HLGS = aimSpeed;
    acceleration_NVH_HLGS = aimAcceleration;
    approachTime_NVH_HLGS_AccelerationChanging = approachTime;
    speedDifference_NVH_HLGS_AccelerationChanging = acceleration_NVH_HLGS * approachTime_NVH_HLGS_AccelerationChanging / 2.0;
    accelerationGain_NVH_HLGS = accelerationGain;

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_HLGS_KeepSpeed = true;
}

void PedalRobotNVH::Slot_NVH_HLGS_SingleAxisSlowMove(bool ifPos)
{
    double speed = Configuration::Instance()->singleAxisBtnRatio[1];
    speed *= Configuration::Instance()->machineParam[5];//单轴运动的速度倍率
    speed *= ifPos? 0.25 : -0.25;

    UnifiedRobot::Instance()->SendSingleAxisMsg(1, speed);
}

void PedalRobotNVH::Slot_NVH_HLGS_Start(double lowAimOV, double upAimOV, double usingTime, double advancedPos)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 计算所需开度和停止时间
    double accLowOV = pdPreTest->GetLimitOpenValue(1, false);
    double accUpOV = pdPreTest->GetLimitOpenValue(1, true);
    aimLowOV_NVH_HLGS = lowAimOV < accLowOV ? accLowOV : lowAimOV;
    aimUpOV_NVH_HLGS = upAimOV > accUpOV ? accUpOV : upAimOV;
    usingTime_NVH_HLGS = usingTime;
    advancedPos_NVH_HLGS = advancedPos;

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_HLGS = true;
}

void PedalRobotNVH::Slot_NVH_PGS_Start(double aimOV)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 计算所需开度和停止时间
    aimOV_NVH_PGS = aimOV;

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_PGS = true;
}

void PedalRobotNVH::Slot_NVH_APS_Start(double lowAimOV, double upAimOV, double usingTime, double advancedPos)
{
    // 停止
    UnifiedRobot::Instance()->SendEnterIdleMsg();

    // 更新控制方式
    RefreshControlMode();

    // 计算所需开度和停止时间
    double accLowOV = pdPreTest->GetLimitOpenValue(1, false);
    double accUpOV = pdPreTest->GetLimitOpenValue(1, true);
    aimLowOV_NVH_APS = lowAimOV < accLowOV ? accLowOV : lowAimOV;
    aimUpOV_NVH_APS = upAimOV > accUpOV ? accUpOV : upAimOV;
    usingTime_NVH_APS = usingTime;
    advancedPos_NVH_APS = advancedPos;

    // 初始化部分参数
    recordDatasNVH.clear();
    recordDatasNVH.reserve(2000);
    caseNVH = 1;

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

    // 开始逻辑控制
    statusNVHRun = 1;
    ifRun_NVH_APS = true;
}




void PedalRobotNVH::Slot_NVH_ConfirmStop()
{
    statusNVHRun = 0;
}

void PedalRobotNVH::Slot_NVH_StopNow()
{
    ImmediateEndNVHCtrl();
    NotImmediateEndNVHCtrl();
}






