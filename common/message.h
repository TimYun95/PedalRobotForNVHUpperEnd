#ifndef MESSAGE_H
#define MESSAGE_H

#include <string>

#include "globalvariables.h"

#include "communication/MsgQueue.h"
#include "communication/libmodbus/EndianConverter.h"

enum manual_direction {
    md_stop,
    md_xplus,
    md_xminus,
    md_yplus,
    md_yminus,
    md_zplus,
    md_zminus,
    md_rxplus,
    md_rxminus,
    md_ryplus,
    md_ryminus,
    md_rzplus,
    md_rzminus
};

enum manual_method{
    mm_jog,
    mm_con
};

struct PauseParameterMsg
{
    bool isResume;
    double pausedPosition[RobotParams::axisNum];
    unsigned int pausedArcPoints;
    unsigned int pausedEllipsePoints;
    unsigned int pausedSectionSNo;
    unsigned int pausedInterPoints;
    double intersection_std_theta;
};

struct ActionMsg {
    int status;
    bool isGunOn;
    PauseParameterMsg pauseParameter;
    unsigned int sectionNo;
    unsigned int pausedNo;
    unsigned int beginSectionNo;
    unsigned int endSectionNo;
    unsigned int nowTimes;
    unsigned int loopTimes;
    double moveToTranspositionSpeed;
    double weldSpeed;
    bool isCheck;
    char mod;
    char lastMod;
    short usage;
    enum tagUsage{
        NormalTeach=0,
        MoveAfterGoHome,
        RequestLoggerFile
    };
    bool operator ==(const struct ActionMsg actMsg) const;

    //don't forget to update the following functions after modified this struct
    std::tr1::shared_ptr<EndianConverter> Serialize() const;
    void Deserialize(unsigned short *addr);
private:
    void Construct(EndianConverter &ec) const;
};

struct ConfigurationMsg {
    int motorMaxPps;
    int angleMutationThreshold;
    int openGunTime;
    int closeGunTime;
    int transPointRatio;

    double limitPos[RobotParams::axisNum];
    double limitNeg[RobotParams::axisNum];

    double motorRatio[RobotParams::axisNum];
    int motorSubdivision[RobotParams::axisNum];

    double encoderRatio[RobotParams::axisNum];
    int encoderSubdivision[RobotParams::axisNum];

    int pulse_filter_limit[RobotParams::axisNum];
    double speedLimit[RobotParams::axisNum];
    double motorAcceleration[RobotParams::axisNum];

    double maxMonitorDiffTheta[RobotParams::axisNum];

    double kp[RobotParams::axisNum];
    double ki[RobotParams::axisNum];
    double kd[RobotParams::axisNum];
    double kf[RobotParams::axisNum];
    double kp_work[RobotParams::axisNum];
    double ki_work[RobotParams::axisNum];
    double kd_work[RobotParams::axisNum];
    double kf_work[RobotParams::axisNum];

    double N1_7or5_ratio;
    double N2_7or5_ratio;
    double N4_7or5_ratio;
    double N6_7or5_ratio;
    double N7_7or5_ratio;

    double goHomeLimit[RobotParams::axisNum];
    double homeAngle[RobotParams::axisNum];
    double goHomeHighSpeed[RobotParams::axisNum];
    double goHomeLowSpeed[RobotParams::axisNum];

    int singleAbsValueAtOrigin[RobotParams::axisNum];
    int multiAbsValueAtOrigin[RobotParams::axisNum];

    int motorPort[RobotParams::axisNum];
    int encoderPort[RobotParams::axisNum];
    int optSwitchPort[RobotParams::axisNum];

    int motorDirection[RobotParams::axisNum];/* 电机的方向 */
    int encoderDirection[RobotParams::axisNum];/* 编码器的方向 */
    int goHomeDirection[RobotParams::axisNum];/* 回原的方向 */

    double errorLimitTrans[RobotParams::axisNum];/* 过渡段误差限 */
    double errorLimitWork[RobotParams::dof];/* 工作段误差限 */

    double machinePara[RobotParams::machineParamNum];
    double reservedParam[RobotParams::reservedParamNum];

    //don't forget to update the following functions after modified this struct
    std::tr1::shared_ptr<EndianConverter> Serialize() const;
    void Deserialize(unsigned short *addr);
private:
    void Construct(EndianConverter& ec) const;
};

struct EditSpeedMsg {
    std::vector<std::pair<int,double> > record;

    //don't forget to update the following two functions after modified this struct
    std::tr1::shared_ptr<EndianConverter> Serialize() const;
    void Deserialize(unsigned short *addr);
private:
    void Construct(EndianConverter& ec) const;
};
#endif
