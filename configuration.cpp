#include "configuration.h"

#include <QDir>

#include "common/printf.h"

#ifdef DOF_3_PEDAL_API
static const std::string RootPath = std::string("./conf/") + std::string(RobotParams::robotFolder);
#else
static const std::string RootPath = QDir::homePath().toStdString() + "/Documents/" + RobotParams::robotFolder;
#endif

Configuration::Configuration()
    : ConfigurationBase()
{
    SetConfFilePath( GetFilePath(RobotConfFilePath) );
}

Configuration::~Configuration()
{
}

std::string Configuration::GetFilePath(Configuration::eFilePathType filePathType)
{
    const std::string &rootPath = RootPath;
    const std::string systemFolderPath = rootPath + "/system_files/";

    switch(filePathType){
    case RootFolderPath:
        return rootPath;
    case SystemFolderPath:
        return systemFolderPath;
    case RobotConfFilePath:
        return systemFolderPath + "configuration.xml";
    case RobotOriginFilePath:
        return systemFolderPath + "origin.txt";
    case RobotSoftStopFilePath:
        return systemFolderPath + "softStop.txt";
    case RobotMonitoringFilePath:
        return systemFolderPath + "monitoringMode.txt";
    default:
        PRINTF(LOG_ERR, "%s: invalid filePathType=[%d]\n", __func__, filePathType);
        return rootPath;
    }
}

void Configuration::DoReadElement(QDomElement &element)
{
    LOAD_NUMBER_ELEMENT(serialDevice);
    LOAD_NUMBER_ELEMENT(motorMaxPps);
    LOAD_NUMBER_ELEMENT(angleMutationThreshold);
    LOAD_NUMBER_ELEMENT(openGunDelay);
    LOAD_NUMBER_ELEMENT(closeGunDelay);
    LOAD_NUMBER_ELEMENT(transSpeed);
    LOAD_NUMBER_ELEMENT(calibratingPoints);
    LOAD_NUMBER_ELEMENT(transPointRatio);

    LOAD_NUMBER_ARRAY(limitPos, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(limitNeg, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(motorRatio, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(motorSubdivision, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(encoderRatio, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(encoderSubdivision, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(pulseFilter, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(speedLimit, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(motorAcceleration, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(singleAxisBtnRatio, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(singleAxisSliderRatio, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(maxMonitorDiffTheta, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(manualSpeedRatio, RobotParams::dof);

    LOAD_NUMBER_ARRAY(kp, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(ki, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(kd, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(kf, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(kp_work, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(ki_work, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(kd_work, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(kf_work, RobotParams::axisNum);

    LOAD_STRING_ELEMENT(normalPassword);
    LOAD_STRING_ELEMENT(rootPassword);
    LOAD_STRING_ELEMENT(defaultFile);
    LOAD_STRING_ELEMENT(filePathError);

    LOAD_NUMBER_ARRAY(pausedPosition, RobotParams::axisNum);
    LOAD_NUMBER_ELEMENT(pausedNo);
    LOAD_NUMBER_ELEMENT(pausedSectionSNo);
    LOAD_NUMBER_ELEMENT(pausedArcPoints);
    LOAD_NUMBER_ELEMENT(pausedInterPoints);
    LOAD_NUMBER_ELEMENT(pausedEllipsePoints);
    LOAD_NUMBER_ELEMENT(intersection_std_theta);

    LOAD_NUMBER_ELEMENT(isErrorEverOccur);
    LOAD_NUMBER_ELEMENT(isWelding);

    LOAD_NUMBER_ELEMENT(N1_7or5_ratio);
    LOAD_NUMBER_ELEMENT(N2_7or5_ratio);
    LOAD_NUMBER_ELEMENT(N4_7or5_ratio);
    LOAD_NUMBER_ELEMENT(N6_7or5_ratio);
    LOAD_NUMBER_ELEMENT(N7_7or5_ratio);

    LOAD_NUMBER_ARRAY(goHomeLimit, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(homeAngle, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(goHomeHighSpeed, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(goHomeLowSpeed, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(singleAbsValue, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(multiAbsValue, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(motorPort, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(encoderPort, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(optSwitchPort, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(motorDirection, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(encoderDirection, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(goHomeDirection, RobotParams::axisNum);

    LOAD_NUMBER_ARRAY(errorLimitTrans, RobotParams::axisNum);
    LOAD_NUMBER_ARRAY(errorLimitWork, RobotParams::dof);

    LOAD_NUMBER_ARRAY(machineParam, RobotParams::machineParamNum);
    LOAD_NUMBER_ARRAY(reservedParam, RobotParams::reservedParamNum);

    LOAD_NUMBER_ELEMENT(forceValueThres);
    LOAD_NUMBER_ELEMENT(balanceForce);

    LOAD_NUMBER_ARRAY(deathOpenValue, 2);
    LOAD_NUMBER_ARRAY(pedalPositionLimit, 2);
    LOAD_NUMBER_ELEMENT(brakeThetaAfterGoHome);
    LOAD_NUMBER_ELEMENT(getSpeedMethod);
    LOAD_NUMBER_ELEMENT(calcSpeedErrorMethod);
    LOAD_NUMBER_ELEMENT(ADCalibrateK);
    LOAD_NUMBER_ELEMENT(pulseSpeedCalibrateMethod);
    LOAD_NUMBER_ELEMENT(pulseCalibrateFrequency);
    LOAD_NUMBER_ELEMENT(pulseCalibrateSpeed);
    LOAD_NUMBER_ELEMENT(pulseCounterSwitchSpeed);
    LOAD_NUMBER_ELEMENT(pedalRobotUsage);
    LOAD_STRING_ELEMENT(defaultCarTypeFile);
    LOAD_NUMBER_ARRAY(sysControlParams, 7);
    LOAD_NUMBER_ARRAY(sysControlParamsWltc, 7);

    LOAD_STRING_ELEMENT(wsServerIP);
    LOAD_STRING_ELEMENT(wsGroupId);
    LOAD_STRING_ELEMENT(wsPassword);
    LOAD_STRING_ELEMENT(wsRsaPublicKeyFilePath);
    LOAD_NUMBER_ELEMENT(wsCmtRole);
}

void Configuration::DoSaveElement(QDomDocument& doc, QDomElement& root)
{
    SAVE_NUMBER_ELEMENT(serialDevice);
    SAVE_NUMBER_ELEMENT(motorMaxPps);
    SAVE_NUMBER_ELEMENT(angleMutationThreshold);
    SAVE_NUMBER_ELEMENT(openGunDelay);
    SAVE_NUMBER_ELEMENT(closeGunDelay);
    SAVE_NUMBER_ELEMENT(transSpeed);
    SAVE_NUMBER_ELEMENT(calibratingPoints);
    SAVE_NUMBER_ELEMENT(transPointRatio);

    SAVE_NUMBER_ARRAY(limitPos,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(limitNeg,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(motorRatio,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(motorSubdivision,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(encoderRatio,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(encoderSubdivision,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(pulseFilter,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(speedLimit,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(motorAcceleration, RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(singleAxisBtnRatio, RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(singleAxisSliderRatio, RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(maxMonitorDiffTheta, RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(manualSpeedRatio, RobotParams::dof);

    SAVE_NUMBER_ARRAY(kp,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(ki,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(kd,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(kf,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(kp_work,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(ki_work,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(kd_work,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(kf_work,RobotParams::axisNum);

    SAVE_STRING_ELEMENT(normalPassword);
    SAVE_STRING_ELEMENT(rootPassword);
    SAVE_STRING_ELEMENT(defaultFile);
    SAVE_STRING_ELEMENT(filePathError);

    SAVE_NUMBER_ARRAY(pausedPosition,RobotParams::axisNum);
    SAVE_NUMBER_ELEMENT(pausedNo);
    SAVE_NUMBER_ELEMENT(pausedSectionSNo);
    SAVE_NUMBER_ELEMENT(pausedArcPoints);
    SAVE_NUMBER_ELEMENT(pausedInterPoints);
    SAVE_NUMBER_ELEMENT(pausedEllipsePoints);
    SAVE_NUMBER_ELEMENT(intersection_std_theta);

    SAVE_NUMBER_ELEMENT(isErrorEverOccur);
    SAVE_NUMBER_ELEMENT(isWelding);

    SAVE_NUMBER_ELEMENT(N1_7or5_ratio);
    SAVE_NUMBER_ELEMENT(N2_7or5_ratio);
    SAVE_NUMBER_ELEMENT(N4_7or5_ratio);
    SAVE_NUMBER_ELEMENT(N6_7or5_ratio);
    SAVE_NUMBER_ELEMENT(N7_7or5_ratio);

    SAVE_NUMBER_ARRAY(goHomeLimit,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(homeAngle,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(goHomeHighSpeed,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(goHomeLowSpeed,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(singleAbsValue,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(multiAbsValue,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(motorPort,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(encoderPort,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(optSwitchPort,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(motorDirection,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(encoderDirection,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(goHomeDirection,RobotParams::axisNum);

    SAVE_NUMBER_ARRAY(errorLimitTrans,RobotParams::axisNum);
    SAVE_NUMBER_ARRAY(errorLimitWork,RobotParams::dof);

    SAVE_NUMBER_ARRAY(machineParam,RobotParams::machineParamNum);
    SAVE_NUMBER_ARRAY(reservedParam,RobotParams::reservedParamNum);

    SAVE_NUMBER_ELEMENT(forceValueThres);
    SAVE_NUMBER_ELEMENT(balanceForce);

    SAVE_NUMBER_ARRAY(deathOpenValue, 2);
    SAVE_NUMBER_ARRAY(pedalPositionLimit, 2);
    SAVE_NUMBER_ELEMENT(brakeThetaAfterGoHome);
    SAVE_NUMBER_ELEMENT(getSpeedMethod);
    SAVE_NUMBER_ELEMENT(calcSpeedErrorMethod);
    SAVE_NUMBER_ELEMENT(ADCalibrateK);
    SAVE_NUMBER_ELEMENT(pulseSpeedCalibrateMethod);
    SAVE_NUMBER_ELEMENT(pulseCalibrateFrequency);
    SAVE_NUMBER_ELEMENT(pulseCalibrateSpeed);
    SAVE_NUMBER_ELEMENT(pulseCounterSwitchSpeed);
    SAVE_NUMBER_ELEMENT(pedalRobotUsage);
    SAVE_STRING_ELEMENT(defaultCarTypeFile);
    SAVE_NUMBER_ARRAY(sysControlParams, 7);
    SAVE_NUMBER_ARRAY(sysControlParamsWltc, 7);
    SAVE_NUMBER_ELEMENT(pedalStartTimeS);

    SAVE_STRING_ELEMENT(wsServerIP);
    SAVE_STRING_ELEMENT(wsGroupId);
    SAVE_STRING_ELEMENT(wsPassword);
    SAVE_STRING_ELEMENT(wsRsaPublicKeyFilePath);
    SAVE_NUMBER_ELEMENT(wsCmtRole);
}

void Configuration::LoadDefaultConfiguration()
{
    serialDevice=0;
    motorMaxPps=200*1000;//200K PPS
    angleMutationThreshold = 20;
    openGunDelay=300;
    closeGunDelay=300;
    transSpeed=30;
    calibratingPoints=200;
    transPointRatio=1000;

    normalPassword="123456";
    rootPassword="hjkl;'";
    defaultFile="/root/Documents/0";
    filePathError="";

    N1_7or5_ratio=0.1;//N1=N3
    N7_7or5_ratio=0.3;//N7=N5
    //N1*2+N7*2 应当小于1，不足1的部分分配给2+4+6
    N2_7or5_ratio=0;//0
    N4_7or5_ratio=1;//1
    N6_7or5_ratio=0;//0
    //N2+N6+N4=1

    for(int i=0; i<RobotParams::axisNum; ++i){
        limitPos[i]=100;
        limitNeg[i]=100;

        motorRatio[i]=100;
        motorSubdivision[i]=1000;

        encoderRatio[i]=100;
        encoderSubdivision[i]=10000;

        pulseFilter[i]=5;
        speedLimit[i]=0.1;
        motorAcceleration[i]=0.01;

        singleAxisBtnRatio[i] = 1;
        singleAxisSliderRatio[i] = 1;

        maxMonitorDiffTheta[i] = 10.0;

        kp[i]=0.1;
        ki[i]=0;
        kd[i]=0;
        kf[i]=0;
        kp_work[i]=0.1;
        ki_work[i]=0;
        kd_work[i]=0;
        kf_work[i]=0;

        goHomeLimit[i]=100;
        homeAngle[i]=0;
        goHomeHighSpeed[i] = 0.5;
        goHomeLowSpeed[i] = 0.1;

        singleAbsValue[i]=10;
        multiAbsValue[i]=100;

        motorPort[i]=i;
        encoderPort[i]=i;
        optSwitchPort[i]=i;

        motorDirection[i]=1;
        encoderDirection[i]=1;
        goHomeDirection[i]=1;

        errorLimitTrans[i]=10;
    }

    for(int i=0; i<RobotParams::dof; ++i){
        errorLimitWork[i]=10;

        manualSpeedRatio[i] = 1;
    }

    for(int i=0; i<RobotParams::machineParamNum; ++i){
        machineParam[i]=10.0;
    }
    for(int i=0; i<RobotParams::reservedParamNum; ++i){
        reservedParam[i]=10.0;
    }

    for(int i=0; i<RobotParams::axisNum; ++i){
        pausedPosition[i]=0;
    }
    pausedNo=0;
    pausedSectionSNo=0;
    pausedArcPoints=0;
    pausedInterPoints=0;
    pausedEllipsePoints=0;
    intersection_std_theta=0;

    isWelding=false;
    isErrorEverOccur=false;

    /* DOF_4_MEDICAL */
    forceValueThres = 4.0;
    balanceForce = 0.0;

    /* DOF_2_PEDAL */
    for(int i=0; i<2; ++i){
        deathOpenValue[i] = 1.0;
        pedalPositionLimit[i] = 80.0;
    }
    brakeThetaAfterGoHome = 50.0;
    getSpeedMethod = 0;
    calcSpeedErrorMethod = 0;
    ADCalibrateK = 20.0;
    pulseSpeedCalibrateMethod = 0;
    pulseCalibrateFrequency = 0.0;
    pulseCalibrateSpeed = 0.0;
    pulseCounterSwitchSpeed = 5.0;
    pedalRobotUsage = 0;
    defaultCarTypeFile = "";
    for(int i=0; i<7; ++i){
        sysControlParams[i] = 0.1;
    }
    for(int i=0; i<5; ++i){
        sysControlParamsWltc[i] = 0.1;
    }

    /* WebSocket通讯 */
    wsServerIP = "192.168.1.115";
    wsGroupId = "Lab1005Group";
    wsPassword = "Lab1005GroupPassword";
    wsRsaPublicKeyFilePath = "E:/OpenSSL-Win32/bin/RSA/test_pub.key";
    wsCmtRole = 0;
}
