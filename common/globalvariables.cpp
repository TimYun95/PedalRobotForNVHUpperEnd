#include "globalvariables.h"
#include <string>
#include "printf.h"

void RobotThetaMatrix::SetTheta(int axisIndex, const double &value)
{
    if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: index(%d) out of range!\n", __func__, axisIndex);
        return;
    }

    m_theta[axisIndex] = value;
}

double RobotThetaMatrix::GetTheta(int axisIndex) const
{
    if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: index(%d) out of range!\n", __func__, axisIndex);
        return 0.0;
    }

    return m_theta[axisIndex];
}

void RobotThetaMatrix::SetMatrix(int dofIndex, const double &value)
{
    if(dofIndex<0 || dofIndex>=RobotParams::dof){
        PRINTF(LOG_ERR, "%s: index(%d) out of range!\n", __func__, dofIndex);
        return;
    }

    m_matrix[dofIndex] = value;
}

double RobotThetaMatrix::GetMatrix(int dofIndex) const
{
    if(dofIndex<0 || dofIndex>=RobotParams::dof){
        PRINTF(LOG_ERR, "%s: index(%d) out of range!\n", __func__, dofIndex);
        return 0.0;
    }

    return m_matrix[dofIndex];
}

RobotThetaMatrix::RobotThetaMatrix()
{
    for(int i=0; i<RobotParams::axisNum; ++i){
        m_theta[i] = 0.0;
    }
    for(int i=0; i<RobotParams::dof; ++i){
        m_matrix[i] = 0.0;
    }
}

RobotThetaMatrix::~RobotThetaMatrix()
{
}

int StatusString::GetIndexForMotorPulseMaxPps(int axisNum)
{
    switch(axisNum){
    case 0:
        return StatusString::MotorPulse_MaxPps_Axis1;
    case 1:
        return StatusString::MotorPulse_MaxPps_Axis2;
    case 2:
        return StatusString::MotorPulse_MaxPps_Axis3;
    case 3:
        return StatusString::MotorPulse_MaxPps_Axis4;
    case 4:
        return StatusString::MotorPulse_MaxPps_Axis5;
    case 5:
        return StatusString::MotorPulse_MaxPps_Axis6;
    default:
        return StatusString::InvalidStatusStringType;
    }
}

int StatusString::GetIndexForPositionLimit(int axisNum, bool isPositivePositionLimit)
{
    if(isPositivePositionLimit){
        switch(axisNum){
        case 0:
            return StatusString::PositivePositionLimit_Axis1;
        case 1:
            return StatusString::PositivePositionLimit_Axis2;
        case 2:
            return StatusString::PositivePositionLimit_Axis3;
        case 3:
            return StatusString::PositivePositionLimit_Axis4;
        case 4:
            return StatusString::PositivePositionLimit_Axis5;
        case 5:
            return StatusString::PositivePositionLimit_Axis6;
        default:
            return StatusString::InvalidStatusStringType;
        }
    }else{
        switch(axisNum){
        case 0:
            return StatusString::NegativePositionLimit_Axis1;
        case 1:
            return StatusString::NegativePositionLimit_Axis2;
        case 2:
            return StatusString::NegativePositionLimit_Axis3;
        case 3:
            return StatusString::NegativePositionLimit_Axis4;
        case 4:
            return StatusString::NegativePositionLimit_Axis5;
        case 5:
            return StatusString::NegativePositionLimit_Axis6;
        default:
            return StatusString::InvalidStatusStringType;
        }
    }
}

int StatusString::GetIndexForGoHomeFailure(int axisNum)
{
    switch(axisNum){
    case 0:
        return StatusString::Error_GoHomeFailure_Axis1;
    case 1:
        return StatusString::Error_GoHomeFailure_Axis2;
    case 2:
        return StatusString::Error_GoHomeFailure_Axis3;
    case 3:
        return StatusString::Error_GoHomeFailure_Axis4;
    case 4:
        return StatusString::Error_GoHomeFailure_Axis5;
    case 5:
        return StatusString::Error_GoHomeFailure_Axis6;
    default:
        return StatusString::InvalidStatusStringType;
    }
}

#ifdef ROBOT_INTERFACE
const QString &StatusString::StatusIndex2String(int statusIndex)
{
    switch(statusIndex){
        case MotorPulse_MaxPps_Axis6:
        {
            static QString stringStatus = QObject::tr(" 6轴脉冲频率超过驱动器最大指令脉冲频率!");
            return stringStatus;
        }
        case MotorPulse_MaxPps_Axis5:
        {
            static QString stringStatus = QObject::tr(" 5轴脉冲频率超过驱动器最大指令脉冲频率!");
            return stringStatus;
        }
        case MotorPulse_MaxPps_Axis4:
        {
            static QString stringStatus = QObject::tr(" 4轴脉冲频率超过驱动器最大指令脉冲频率!");
            return stringStatus;
        }
        case MotorPulse_MaxPps_Axis3:
        {
            static QString stringStatus = QObject::tr(" 3轴脉冲频率超过驱动器最大指令脉冲频率!");
            return stringStatus;
        }
        case MotorPulse_MaxPps_Axis2:
        {
            static QString stringStatus = QObject::tr(" 2轴脉冲频率超过驱动器最大指令脉冲频率!");
            return stringStatus;
        }
        case MotorPulse_MaxPps_Axis1:
        {
            static QString stringStatus = QObject::tr(" 1轴脉冲频率超过驱动器最大指令脉冲频率!");
            return stringStatus;
        }
        case NegativePositionLimit_Axis6:
        {
            static QString stringStatus = QObject::tr("6轴到达负限位，请停止6轴负方向运动");
            return stringStatus;
        }
        case NegativePositionLimit_Axis5:
        {
            static QString stringStatus = QObject::tr("5轴到达负限位，请停止5轴负方向运动");
            return stringStatus;
        }
        case NegativePositionLimit_Axis4:
        {
            static QString stringStatus = QObject::tr("4轴到达负限位，请停止4轴负方向运动");
            return stringStatus;
        }
        case NegativePositionLimit_Axis3:
        {
            static QString stringStatus = QObject::tr("3轴到达负限位，请停止3轴负方向运动");
            return stringStatus;
        }
        case NegativePositionLimit_Axis2:
        {
            static QString stringStatus = QObject::tr("2轴到达负限位，请停止2轴负方向运动");
            return stringStatus;
        }
        case NegativePositionLimit_Axis1:
        {
            static QString stringStatus = QObject::tr("1轴到达负限位，请停止1轴负方向运动");
            return stringStatus;
        }
        case InvalidStatusStringType:
        {
            static QString stringStatus = QObject::tr("无效的状态(系统初始化!)");
            return stringStatus;
        }
        case PositivePositionLimit_Axis1:
        {
            static QString stringStatus = QObject::tr("1轴到达正限位，请停止1轴正方向运动");
            return stringStatus;
        }
        case PositivePositionLimit_Axis2:
        {
            static QString stringStatus = QObject::tr("2轴到达正限位，请停止2轴正方向运动");
            return stringStatus;
        }
        case PositivePositionLimit_Axis3:
        {
            static QString stringStatus = QObject::tr("3轴到达正限位，请停止3轴正方向运动");
            return stringStatus;
        }
        case PositivePositionLimit_Axis4:
        {
            static QString stringStatus = QObject::tr("4轴到达正限位，请停止4轴正方向运动");
            return stringStatus;
        }
        case PositivePositionLimit_Axis5:
        {
            static QString stringStatus = QObject::tr("5轴到达正限位，请停止5轴正方向运动");
            return stringStatus;
        }
        case PositivePositionLimit_Axis6:
        {
            static QString stringStatus = QObject::tr("6轴到达正限位，请停止6轴正方向运动");
            return stringStatus;
        }
        case ActionError_WrongTeachFileType:
        {
            static QString stringStatus = QObject::tr("示教文件类型不适用本机器！");
            return stringStatus;
        }
        case ActionStop_ManualStopAction:
        {
            static QString stringStatus = QObject::tr("您终止了示教操作，未能完成示教任务!");
            return stringStatus;
        }
        case ActionError_SafeCheckWarning:
        {
            static QString stringStatus = QObject::tr("故障发生，示教中断,请结结束示教");
            return stringStatus;
        }
        case ActionError_EndEffectorWarning:
        {
            static QString stringStatus = QObject::tr("故障发生，焊枪过热，请结束示教");
            return stringStatus;
        }
        case ActionStatus_ManualPauseAction:
        {
            static QString stringStatus = QObject::tr("您选择了暂停示教进程");
            return stringStatus;
        }
        case ActionStatus_MovingToDotSection:
        {
            static QString stringStatus = QObject::tr("正在移动到点段...");
            return stringStatus;
        }
        case ActionStatus_RunningDotSection:
        {
            static QString stringStatus = QObject::tr("正在进行点段示教...");
            return stringStatus;
        }
        case ActionStatus_RunningLogicSection:
        {
            static QString stringStatus = QObject::tr("正在进行逻辑段...");
            return stringStatus;
        }
        case ActionStatus_MovingToFirstPoint:
        {
            static QString stringStatus = QObject::tr("正在移动到第一点...");
            return stringStatus;
        }
        case ActionStatus_RunningInteractionSection:
        {
            static QString stringStatus = QObject::tr("正在进行I示教...");
            return stringStatus;
        }
        case ActionStatus_MovingToFirstPointOfEllipseSection:
        {
            static QString stringStatus = QObject::tr("正在移动到椭弧线第一点...");
            return stringStatus;
        }
        case ActionStatus_RunningEllipseSection:
        {
            static QString stringStatus = QObject::tr("正在进行Ellipse示教...");
            return stringStatus;
        }
        case ActionStatus_MovingToFirstPointOfLineSection:
        {
            static QString stringStatus = QObject::tr("正在移动到直线第一点...");
            return stringStatus;
        }
        case ActionStatus_RunningLineSection:
        {
            static QString stringStatus = QObject::tr("正在进行直线示教...");
            return stringStatus;
        }
        case ActionStatus_MovingToFirstPointOfCircleSection:
        {
            static QString stringStatus = QObject::tr("正在移动到弧线第一点...");
            return stringStatus;
        }
        case ActionStatus_RunningCircleSection:
        {
            static QString stringStatus = QObject::tr("正在进行弧线示教...");
            return stringStatus;
        }
        case ActionStatus_RunningTransSection:
        {
            static QString stringStatus = QObject::tr("正在移动到过渡点...");
            return stringStatus;
        }
        case ActionStatus_MovingToFirstPointOfSegmentSection:
        {
            static QString stringStatus = QObject::tr("正在移动到多段曲线第一点...");
            return stringStatus;
        }
        case ActionStatus_RunningSegmentSection:
        {
            static QString stringStatus = QObject::tr("正在进行多段曲线示教...");
            return stringStatus;
        }
        case ActionStatus_MovingToWaitingPoint:
        {
            static QString stringStatus = QObject::tr("正在移动到等待点...");
            return stringStatus;
        }
        case ActionStatus_ReachWaitingPoint:
        {
            static QString stringStatus = QObject::tr("到达等待点，等待继续...");
            return stringStatus;
        }
        case ActionStatus_RunningMonitorSection:
        {
            static QString stringStatus = QObject::tr("正在进行监听模式示教...");
            return stringStatus;
        }
        case ActionStatus_FinishAllAction:
        {
            static QString stringStatus = QObject::tr("整个示教过程结束！");
            return stringStatus;
        }
        case Error_CannotCalcCircle:
        {
            static QString stringStatus = QObject::tr("示教点无法组成圆弧！");
            return stringStatus;
        }
        case Error_CannotCalcEllipse:
        {
            static QString stringStatus = QObject::tr("无法解得椭圆方程！");
            return stringStatus;
        }
        case Error_PleaseMoveOppositely:
        {
            static QString stringStatus = QObject::tr("请向反方向运动");
            return stringStatus;
        }
        case Error_AbsEncoderUnstable:
        {
            static QString stringStatus = QObject::tr("伺服读取绝对编码器数据不稳定! 请待电机停稳后进行回原");
            return stringStatus;
        }
        case Error_FinishGoHome:
        {
            static QString stringStatus = QObject::tr("已经到达原点，可以进行其他操作。");
            return stringStatus;
        }
        case Error_ManualCancelGoHome:
        {
            static QString stringStatus = QObject::tr("您终止了回原点操作,未到达原点!");
            return stringStatus;
        }
        case Error_GoHomeFailure_Axis1:
        {
            static QString stringStatus = QObject::tr("1号轴返回原点失败！请检查光电开关");
            return stringStatus;
        }
        case Error_GoHomeFailure_Axis2:
        {
            static QString stringStatus = QObject::tr("2号轴返回原点失败！请检查光电开关");
            return stringStatus;
        }
        case Error_GoHomeFailure_Axis3:
        {
            static QString stringStatus = QObject::tr("3号轴返回原点失败！请检查光电开关");
            return stringStatus;
        }
        case Error_GoHomeFailure_Axis4:
        {
            static QString stringStatus = QObject::tr("4号轴返回原点失败！请检查光电开关");
            return stringStatus;
        }
        case Error_GoHomeFailure_Axis5:
        {
            static QString stringStatus = QObject::tr("5号轴返回原点失败！请检查光电开关");
            return stringStatus;
        }
        case Error_GoHomeFailure_Axis6:
        {
            static QString stringStatus = QObject::tr("6号轴返回原点失败！请检查光电开关");
            return stringStatus;
        }
        case Error_GoHomeFailure_AxisNotMove:
        {
            static QString stringStatus = QObject::tr("返回原点失败！请检查相关电路");
            return stringStatus;
        }
        case Error_GoHomeFailure_BadAxisPosition:
        {
            static QString stringStatus = QObject::tr("返回原点失败,回原点之前请将轴置于合适位置！");
            return stringStatus;
        }
        case Error_MotorDriverWarning:
        {
            static QString stringStatus = QObject::tr(" 驱动器警报！");
            return stringStatus;
        }
        case Error_EndEffectorWarning:
        {
            static QString stringStatus = QObject::tr(" 焊枪过热！");
            return stringStatus;
        }
        case Error_EndEffectorCollide:
        {
            static QString stringStatus = QObject::tr(" 焊枪发生碰撞！");
            return stringStatus;
        }
        case Status_InitedAndNotGoHomed:
        {
            static QString stringStatus = QObject::tr("机器人刚开启，返回原点前，限位和其它操作无效！");
            return stringStatus;
        }
        case Status_GoHomeInProgress:
        {
            static QString stringStatus = QObject::tr("正在返回原点.....请勿进行其他操作！");
            return stringStatus;
        }
        case Status_AutoGoHomeInProgress:
        {
            static QString stringStatus = QObject::tr("正在自动返回原点.....请勿进行其他操作！");
            return stringStatus;
        }
        case Status_ManualStopAction:
        {
            static QString stringStatus = QObject::tr("您选择了中断所有示教进程");
            return stringStatus;
        }
        case Status_ManualStopRobot:
        {
            static QString stringStatus = QObject::tr("您选择了停止机器的所有运动");
            return stringStatus;
        }
        case Status_DetectCollision:
        {
            static QString stringStatus = QObject::tr(" 检测到碰撞，已中断所有进程");
            return stringStatus;
        }
        case Status_ManualControlPositionLimit:
        {
            static QString stringStatus = QObject::tr("手动控制目标点限位");
            return stringStatus;
        }
        case TeachStatus_FinishConstructTeachSection:
        {
            static QString stringStatus = QObject::tr("示教完成！");
            return stringStatus;
        }
        case TeachStatus_StartConstructTeachSection:
        {
            static QString stringStatus = QObject::tr("开始示教，按“结束示教”退出示教并继续运行！");
            return stringStatus;
        }
        case TeachStatus_ExistSingularPoint:
        {
            static QString stringStatus = QObject::tr("存在奇异点!");
            return stringStatus;
        }
        case TeachStatus_NotExistSingularPoint:
        {
            static QString stringStatus = QObject::tr("不存在奇异点!");
            return stringStatus;
        }
        case TeachStatus_InsertTeachSection:
        {
            static QString stringStatus = QObject::tr("请插入一段示教，该段将插入到选定段之前！");
            return stringStatus;
        }
        case TeachStatus_CancelInsertTeachSection:
        {
            static QString stringStatus = QObject::tr("已取消插入模式！");
            return stringStatus;
        }
        case Status_ConnectiongCommunication:
        {
            static QString stringStatus = QObject::tr("正在连接...");
            return stringStatus;
        }
        case Status_RobotIsClosed:
        {
            static QString stringStatus = QObject::tr("机器已关机!请打开机器建立连接!");
            return stringStatus;
        }
        case Status_LostCommunication:
        {
            static QString stringStatus = QObject::tr("丢失与机器的通讯连接!请检查线路连接并重启机器!");
            return stringStatus;
        }
        case Status_LostMonitorSectionControlMsg:
        {
            static QString stringStatus = QObject::tr("未收到监听指令!机器人已停止!");
            return stringStatus;
        }
        case Status_LostVelocitySectionControlMsg:
        {
            static QString stringStatus = QObject::tr("未收到速度指令!机器人已停止!");
            return stringStatus;
        }
        case Status_EmergencyStopButtonPressed:
        {
            static QString stringStatus = QObject::tr("已按下急停按钮!");
            return stringStatus;
        }
        case Status_EmergencyStopButtonPressedWithoutAction:
        {
            static QString stringStatus = QObject::tr("已按下急停按钮! 但机器人没有回原!");
            return stringStatus;
        }
    }

    static QString stringStatus = QObject::tr("未定义的状态!");
    return stringStatus;
}

const QString &StatusString::GetStatusString()
{
    int statusIndex = StatusString::Instance()->GetStatusIndex();
    return StatusIndex2String(statusIndex);
}
#endif

void StatusString::SetStatusIndex(int statusIndex)
{
    m_statusStringIndex = statusIndex;
}

int StatusString::GetStatusIndex()
{
    return m_statusStringIndex;
}

StatusString::StatusString()
{
    m_statusStringIndex = MaxStatusStringType;
}

StatusString::~StatusString()
{
}
