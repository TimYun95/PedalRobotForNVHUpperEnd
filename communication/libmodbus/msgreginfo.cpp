#include "msgreginfo.h"

#include "communication/msgtypes.h"

unsigned short *tab_registers;

/* message_info describes how many registers each message occupies from which address,
 * new messages must be added in the following struct.
 *    0-2048: C2I 已使用=0-60
 * 2048-5120: I2C 已使用=2048-3174
 * 5120-7168: Both: I2C or C2I 已使用=5120-5406
 * 7168-End : FileLength + FileContent
 */
message_info msgInfo[TAB_BITS_LEN]={
/*    msgType,               起始地址addr,   长度nregs*/
/* 0*/	{0,                       0,            0},
/* 1*/	{C2I_ShowAlarm,           0,            0},
/* 2*/	{C2I_AtSuspendPoint,      0,            0},
/* 3*/	{C2I_FinishAction,        0,            0},
/* 4*/	{C2I_StopWeld,            0,            0},
/* 5*/	{C2I_UpdateTeachFile,     7170,         TAB_REGISTERS_LEN-7170},//File 7170-End
/* 6*/	{C2I_EnterIdleState,      0,            0},
/* 7*/	{C2I_AutoGoHome,          0,            0},
/* 8*/	{C2I_MatrixTheta,         0,            52},//C2I  0-52
/* 9*/	{C2I_FinishGoHome,        52,           2},//C2I   52-54
/*10*/	{C2I_FileContent,         0,            0},
/*11*/	{C2I_ActionStatus,        5120,         90},//Both 5120-5400(预留部分)
/*12*/	{C2I_StringStatus,        54,           2},//C2I   54-56
/*13*/	{C2I_FileLength,          7168,         2},//File  7168-7170
/*14*/	{C2I_OutStatus,           56,           2},//C2I   56-58
/*15*/	{C2I_InStatus,            58,           2},//C2I   58-60
/*16*/	{C2I_ReInit,              0,            0},
/*17*/	{C2I_MessageInform,       5400,         6},//Both  5400-5420(预留部分)
/*18*/	{C2I_UnifiedInform,       5420,         60},//C2I  5420-5480
/*19*/	{0,0,0},
/*20*/	{0,0,0},
/*21*/	{0,0,0},
/*22*/	{0,0,0},
/*23*/	{0,0,0},
/*24*/	{0,0,0},
/*25*/	{0,0,0},
/*26*/	{0,0,0},
/*27*/	{0,0,0},
/*28*/	{0,0,0},
/*29*/	{0,0,0},
/*30*/	{0,0,0},
/*31*/	{0,0,0},
/*32*/	{0,0,0},
/*33*/	{0,0,0},
/*34*/	{0,0,0},
/*35*/	{0,0,0},
/*36*/	{0,0,0},
/*37*/	{0,0,0},
/*38*/	{0,0,0},
/*39*/	{0,0,0},
/*40*/	{0,0,0},
/*41*/	{0,0,0},
/*42*/	{0,0,0},
/*43*/	{0,0,0},
/*44*/	{0,0,0},
/*45*/	{0,0,0},
/*46*/	{0,0,0},
/*47*/	{0,0,0},
/*48*/	{0,0,0},
/*49*/	{0,0,0},
/*50*/	{I2C_StopPid,             0,            0},
/*51*/	{I2C_GoHome,              0,            0},
/*52*/	{I2C_Matrix,              0,            0},
/*53*/	{I2C_ManualSpeed,         2048,         4},//I2C   2048-2052
/*54*/	{I2C_FileContent,         7170,         TAB_REGISTERS_LEN-7170},//File 7170-End
/*55*/	{I2C_SlowDown,            2052,         2},//I2C   2052-2054
/*56*/	//{I2C_Configuration,       2054,         934},//I2C 2054-3054(预留部分)
/*56*/	{I2C_Configuration,       7170,         1022},//I2C 复用I2C_FileContent部分(注意I2C_Configuration和I2C_FileContent不能同时发送)
/*57*/	{I2C_ActionStart,         5120,         90},//Both 5120-5400(预留部分)
/*58*/	{I2C_ManualControl,       3054,         8},//I2C   3054-3062
/*59*/	{I2C_ActionPause,         0,            0},
/*60*/	{I2C_SingleAxisSpeed0,    3062,         6},//I2C   3062-3068
/*61*/	{I2C_SingleAxisSpeed1,    3068,         6},//I2C   3068-3074
/*62*/	{I2C_SingleAxisSpeed2,    3074,         6},//I2C   3074-3080
/*63*/	{I2C_SingleAxisSpeed3,    3080,         6},//I2C   3080-3086
/*64*/	{I2C_SingleAxisSpeed4,    3086,         6},//I2C   3086-3092
/*65*/	{I2C_SingleAxisSpeed5,    3092,         6},//I2C   3092-3098
/*66*/	{I2C_EnterIdleState,      0,			0},
/*67*/	{I2C_ExitWaitPoint,       0,			0},
/*68*/	{I2C_PrintLog,            0,			0},
/*69*/	{I2C_FileLength,          7168,         2},//File  7168-7170
/*70*/	{I2C_ActionResume,        0,			0},
/*71*/	{I2C_ServoOn,             0,			0},
/*72*/	{I2C_OutControl,          3098,         2},//I2C   3098-3100
/*73*/	{I2C_ForceControl,        3100,         4},//I2C   3100-3104
/*74*/	{I2C_Echo,                0,            0},
/*75*/	{I2C_MonitoringCommand,   3104,         38},//I2C  3104-3154(预留部分)
/*76*/	{I2C_PedalOpenValue,      3154,         8},//I2C   3154-3174(预留部分)
/*77*/	{I2C_MessageInform,       5400,         6},//Both  5400-5420(预留部分)
/*78*/	{I2C_VelocityCommand,     5420,         24},//I2C  5420-5470(预留部分)
/*79*/	{0,0,0},
/*80*/	{0,0,0},
/*81*/	{0,0,0},
/*82*/	{0,0,0},
/*83*/	{0,0,0},
/*84*/	{0,0,0},
/*85*/	{0,0,0},
/*86*/	{0,0,0},
/*87*/	{0,0,0},
/*88*/	{0,0,0},
/*89*/	{0,0,0},
/*90*/	{0,0,0},
/*91*/	{0,0,0},
/*92*/	{0,0,0},
/*93*/	{0,0,0},
/*94*/	{0,0,0},
/*95*/	{0,0,0},
/*96*/	{0,0,0},
/*97*/	{0,0,0},
/*98*/	{0,0,0},
/*99*/	{I2C_Shutdown,            0,            0},
};
