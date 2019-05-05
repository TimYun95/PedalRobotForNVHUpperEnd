#ifndef MSGTYPES_H
#define MSGTYPES_H

/* define message type below,
 * C2I: 0~49   Controller to Interface
 * I2C: 50~99  Interface to Controller */
enum tagMessageType{//范围=0~49 即CommunicationMediator中CommunicationDirection的上行消息(下位机 到 上位机)
    C2I_ShowAlarm = 1,
    C2I_AtSuspendPoint,
    C2I_FinishAction,
    C2I_StopWeld,
    C2I_UpdateTeachFile,
    C2I_EnterIdleState,
    C2I_AutoGoHome,
    C2I_MatrixTheta,
    C2I_FinishGoHome,
    C2I_FileContent,
    C2I_ActionStatus,
    C2I_StringStatus,
    C2I_FileLength,
    C2I_OutStatus,
    C2I_InStatus,
    C2I_ReInit,
    C2I_MessageInform,
    C2I_UnifiedInform,

    I2C_StopPid = 50,//范围=50~99 即CommunicationMediator中CommunicationDirection的下行消息(上位机 到 下位机)
    I2C_GoHome,
    I2C_Matrix,
    I2C_ManualSpeed,
    I2C_FileContent,
    I2C_SlowDown,
    I2C_Configuration,
    I2C_ActionStart,
    I2C_ManualControl,
    I2C_ActionPause,
    I2C_SingleAxisSpeed0,
    I2C_SingleAxisSpeed1,
    I2C_SingleAxisSpeed2,
    I2C_SingleAxisSpeed3,
    I2C_SingleAxisSpeed4,
    I2C_SingleAxisSpeed5,
    I2C_EnterIdleState,
    I2C_ExitWaitPoint,
    I2C_PrintLog,
    I2C_FileLength,
    I2C_ActionResume,
    I2C_ServoOn,
    I2C_OutControl,
    I2C_ForceControl,
    I2C_Echo,
    I2C_MonitoringCommand,
    I2C_PedalOpenValue,
    I2C_MessageInform,
    I2C_VelocityCommand,
    I2C_EditWeldSpeed,
    I2C_Shutdown = 99
};

#endif // MSGTYPES_H
