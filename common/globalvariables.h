#ifndef GLOBALVARIABLES_H
#define GLOBALVARIABLES_H

#ifdef ROBOT_INTERFACE
#include <QObject>
#include <QString>
#endif

#include "robotparams.h"
#include "template/singleton.h"

class RobotThetaMatrix : public Singleton<RobotThetaMatrix>
{
public:
    void SetTheta(int axisIndex, const double &value);
    double GetTheta(int axisIndex) const;

    void SetMatrix(int dofIndex, const double &value);
    double GetMatrix(int dofIndex) const;

public:
    RobotThetaMatrix();
    virtual ~RobotThetaMatrix();

private:
    double m_theta[RobotParams::axisNum];//degree/mm
    double m_matrix[RobotParams::dof];//rad/mm
};

class StatusString : public Singleton<StatusString>
{
public:
    enum StatusStringType {
        MotorPulse_MaxPps_Axis6,
        MotorPulse_MaxPps_Axis5,
        MotorPulse_MaxPps_Axis4,
        MotorPulse_MaxPps_Axis3,
        MotorPulse_MaxPps_Axis2,
        MotorPulse_MaxPps_Axis1,

        NegativePositionLimit_Axis6,
        NegativePositionLimit_Axis5,
        NegativePositionLimit_Axis4,
        NegativePositionLimit_Axis3,
        NegativePositionLimit_Axis2,
        NegativePositionLimit_Axis1,

        InvalidStatusStringType,

        PositivePositionLimit_Axis1,
        PositivePositionLimit_Axis2,
        PositivePositionLimit_Axis3,
        PositivePositionLimit_Axis4,
        PositivePositionLimit_Axis5,
        PositivePositionLimit_Axis6,

        ActionError_WrongTeachFileType,
        ActionStop_ManualStopAction,
        ActionError_SafeCheckWarning,
        ActionError_EndEffectorWarning,

        ActionStatus_ManualPauseAction,
        ActionStatus_MovingToDotSection,
        ActionStatus_RunningDotSection,
        ActionStatus_RunningLogicSection,
        ActionStatus_MovingToFirstPoint,
        ActionStatus_RunningInteractionSection,
        ActionStatus_MovingToFirstPointOfEllipseSection,
        ActionStatus_RunningEllipseSection,
        ActionStatus_MovingToFirstPointOfLineSection,
        ActionStatus_RunningLineSection,
        ActionStatus_MovingToFirstPointOfCircleSection,
        ActionStatus_RunningCircleSection,
        ActionStatus_RunningTransSection,
        ActionStatus_MovingToFirstPointOfSegmentSection,
        ActionStatus_RunningSegmentSection,
        ActionStatus_MovingToWaitingPoint,
        ActionStatus_ReachWaitingPoint,
        ActionStatus_RunningMonitorSection,
        ActionStatus_FinishAllAction,

        Error_CannotCalcCircle,
        Error_CannotCalcEllipse,
        Error_PleaseMoveOppositely,
        Error_AbsEncoderUnstable,
        Error_FinishGoHome,
        Error_ManualCancelGoHome,

        Error_GoHomeFailure_Axis1,
        Error_GoHomeFailure_Axis2,
        Error_GoHomeFailure_Axis3,
        Error_GoHomeFailure_Axis4,
        Error_GoHomeFailure_Axis5,
        Error_GoHomeFailure_Axis6,
        Error_GoHomeFailure_AxisNotMove,
        Error_GoHomeFailure_BadAxisPosition,

        Error_MotorDriverWarning,
        Error_EndEffectorWarning,
        Error_EndEffectorCollide,

        Status_InitedAndNotGoHomed,
        Status_GoHomeInProgress,
        Status_AutoGoHomeInProgress,
        Status_ManualStopAction,
        Status_ManualStopRobot,
        Status_DetectCollision,
        Status_ManualControlPositionLimit,

        TeachStatus_FinishConstructTeachSection,
        TeachStatus_StartConstructTeachSection,
        TeachStatus_ExistSingularPoint,
        TeachStatus_NotExistSingularPoint,
        TeachStatus_InsertTeachSection,
        TeachStatus_CancelInsertTeachSection,

        Status_ConnectiongCommunication,
        Status_RobotIsClosed,
        Status_LostCommunication,
        Status_LostMonitorSectionControlMsg,
        Status_LostVelocitySectionControlMsg,
        Status_EmergencyStopButtonPressed,
        Status_EmergencyStopButtonPressedWithoutAction,

        MaxStatusStringType
    };

public:
    static int GetIndexForMotorPulseMaxPps(int axisNum);
    static int GetIndexForPositionLimit(int axisNum, bool isPositivePositionLimit);
    static int GetIndexForGoHomeFailure(int axisNum);

#ifdef ROBOT_INTERFACE
    static const QString& StatusIndex2String(int statusIndex);
    static const QString& GetStatusString();
#endif

public:
    void SetStatusIndex(int statusIndex);
    int GetStatusIndex();

public:
    StatusString();
    virtual ~StatusString();

private:
    int m_statusStringIndex;
};

#endif // GLOBALVARIABLES_H
