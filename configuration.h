#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include "xml/configurationbase.h"
#include "template/singleton.h"
#include "common/robotparams.h"

/* 增加Configuration中的public成员变量后
 * 1. 修改ConfigurationMsg::Serialize() & Deserialize()
 * 2. 修改message_info中的modbus寄存器分布
 * 3. 修改WebSocketCommonMsg::SerializeConfMsg() & DeserializeConfMsg()
 * 4. 修改URI::SendConfiguration() 赋值相应变量*/

class Configuration : public ConfigurationBase, public Singleton<Configuration>
{
public:
    explicit Configuration();
    virtual ~Configuration();

public:
    enum eFilePathType{
        RootFolderPath,
        SystemFolderPath,

        RobotConfFilePath,
        RobotOriginFilePath,
        RobotSoftStopFilePath,
        RobotMonitoringFilePath,

        MaxFilePathType
    };
    std::string GetFilePath(eFilePathType filePathType);

    virtual void LoadDefaultConfiguration() override;
protected:
    virtual void DoReadElement(QDomElement& element) override;
    virtual void DoSaveElement(QDomDocument& doc, QDomElement& root) override;

public:
    int serialDevice;
    int motorMaxPps;
    int angleMutationThreshold;
    int openGunDelay;
    int closeGunDelay;
    int transSpeed;
    int calibratingPoints;
    int transPointRatio;

    double limitPos[RobotParams::axisNum];
    double limitNeg[RobotParams::axisNum];

    double motorRatio[RobotParams::axisNum];
    int motorSubdivision[RobotParams::axisNum];

    double encoderRatio[RobotParams::axisNum];
    int encoderSubdivision[RobotParams::axisNum];

    int pulseFilter[RobotParams::axisNum];
    double speedLimit[RobotParams::axisNum];
    double motorAcceleration[RobotParams::axisNum];

    double singleAxisBtnRatio[RobotParams::axisNum];
    double singleAxisSliderRatio[RobotParams::axisNum];

    double maxMonitorDiffTheta[RobotParams::axisNum];

    double manualSpeedRatio[RobotParams::dof];

    double kp[RobotParams::axisNum];
    double ki[RobotParams::axisNum];
    double kd[RobotParams::axisNum];
    double kf[RobotParams::axisNum];
    double kp_work[RobotParams::axisNum];
    double ki_work[RobotParams::axisNum];
    double kd_work[RobotParams::axisNum];
    double kf_work[RobotParams::axisNum];

    std::string normalPassword;
    std::string rootPassword;
    std::string defaultFile;
    std::string filePathError;

    double pausedPosition[RobotParams::axisNum];
    unsigned int pausedNo;
    unsigned int pausedSectionSNo;
    unsigned int pausedArcPoints;
    unsigned int pausedInterPoints;
    unsigned pausedEllipsePoints;
    double intersection_std_theta;

    bool isErrorEverOccur;
    bool isWelding;

    double N1_7or5_ratio;
    double N2_7or5_ratio;
    double N4_7or5_ratio;
    double N6_7or5_ratio;
    double N7_7or5_ratio;

    double goHomeLimit[RobotParams::axisNum];
    double homeAngle[RobotParams::axisNum];

    int singleAbsValue[RobotParams::axisNum];/*伺服电机 单圈数据*/
    int multiAbsValue[RobotParams::axisNum];/*伺服电机 多圈数据*/

    double goHomeHighSpeed[RobotParams::axisNum];/*光电回原的高速速度*/
    double goHomeLowSpeed[RobotParams::axisNum];/*光电回原的低速速度*/

    int motorPort[RobotParams::axisNum];/* 电机在转接板上的接口0~5 */
    int encoderPort[RobotParams::axisNum];/* 编码器在转接板上的接口0~5 */
    int optSwitchPort[RobotParams::axisNum];/* 光电开关在转接板上的接口0~7 */

    int motorDirection[RobotParams::axisNum];/* 电机的方向 */
    int encoderDirection[RobotParams::axisNum];/* 编码器的方向 */
    int goHomeDirection[RobotParams::axisNum];/* 回原的方向 */

    double errorLimitTrans[RobotParams::axisNum];/* 过渡段误差限 */
    double errorLimitWork[RobotParams::dof];/* 工作段误差限 */

    double machineParam[RobotParams::machineParamNum];/* 机器结构参数 */
    double reservedParam[RobotParams::reservedParamNum];/* 预留参数 */

    /* dof_4_medical */
    double forceValueThres;
    double balanceForce;

    /* dof_2_pedal */
    double deathOpenValue[2];
    double pedalPositionLimit[2];
    double brakeThetaAfterGoHome;
    int getSpeedMethod;
    int calcSpeedErrorMethod;
    double ADCalibrateK;
    int pulseSpeedCalibrateMethod;
    double pulseCalibrateFrequency;
    double pulseCalibrateSpeed;
    double pulseCounterSwitchSpeed;
    int pedalRobotUsage;//程序控制的用途切换
    std::string defaultCarTypeFile;
    double sysControlParams[7];//NEDC学习的参数
    double sysControlParamsWltc[7];//WLTC学习的参数
    double pedalStartTimeS;//调试起始时间

    /* WebSocket通讯 */
    std::string wsServerIP;
    std::string wsGroupId;
    std::string wsPassword;
    std::string wsRsaPublicKeyFilePath;
    int wsCmtRole;
};

#endif // CONFIGURATION_H
