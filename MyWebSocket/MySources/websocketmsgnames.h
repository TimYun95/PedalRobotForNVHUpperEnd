#ifndef WEBSOCKETMSGNAMES_H
#define WEBSOCKETMSGNAMES_H

/* JSON数据序列化时 使用的keyName定义 */

#define WS_MSG_TYPE "wsMsgType"

#define WS_SINGLE_AXIS_Index "wsSingleAxisIndex"
#define WS_SINGLE_AXIS_SPEED   "wsSingleAxisSpeed"

#define WS_IN_STATUS           "wsInStatus"

#define WS_STRING_STATUS       "wsStringStatus"

#define WS_SLOW_DOWN_CHANNEL   "wsSlowDownChannel"

#define WS_OUT_STATUS          "wsOutStatus"

#define WS_OUT_CONTROL_STATUS  "wsOutControlStatus"

#define WS_MANUAL_CONTROL_METHOD "wsManualControlMethod"
#define WS_MANUAL_CONTROL_DIRECTION "wsManualControlDirection"
#define WS_MANUAL_CONTROL_SPEED "wsManualControlSpeed"

#define WS_GO_HOME_RESULT "wsGoHomeResult"

#define WS_MATRIX "wsMatrix"
#define WS_THETA "wsTheta"

#define WS_EDIT_WELD_SPEED "wsEditWeldSpeed"

#define WS_FILE_CONTENT "wsFileContent"

#define WS_MONITOR_COMMAND_METHOD "wsMonitorCommandMethod"
#define WS_MONITOR_COMMAND_THETA "wsMonitorCommandTheta"
#define WS_MONITOR_COMMAND_PARAM "wsMonitorCommandParam"

//特殊的Pedal机器人需要的指令
#define WS_PEDAL_ACC_OPEN_VALUE "wsPedalAccOpenValue"
#define WS_PEDAL_BRAKE_OPEN_VALUE "wsPedalBrakeOpenValue"
//特殊的Medicla机器人需要的指令
#define WS_MEDICAL_FORCE_VALUE "wsMedicalForceValue"

#define WS_MESSAGE_INFORM_TYPE "wsMessageInformType"
#define WS_MESSAGE_INFORM_CONTENT "wsMessageInformContent"

//ActionMsg
#define WS_ACTMSG_IS_RESUME "wsActMsgIsResume"
#define WS_ACTMSG_PAUSED_POSITION "wsActMsgPausedPosition"
#define WS_ACTMSG_PAUSED_ARC_POINTS "wsActMsgPausedArcPoints"
#define WS_ACTMSG_PAUSED_ELLIPSE_POINTS "wsActMsgPausedEllipsePoints"
#define WS_ACTMSG_PAUSED_SECTIONS_NO "wsActMsgPausedSectionSNo"
#define WS_ACTMSG_PAUSED_INTER_POINTS "wsActMsgPausedInterPoints"
#define WS_ACTMSG_PAUSED_INTERSECTION_STD_THETA "wsActMsgIntersectionStdTheta"

#define WS_ACTMSG_STATUS "wsActMsgStatus"
#define WS_ACTMSG_IS_GUN_ON "wsActMsgIsGunOn"
#define WS_ACTMSG_SECTION_NO "wsActMsgSectionNo"
#define WS_ACTMSG_PAUSED_NO "wsActMsgPausedNo"
#define WS_ACTMSG_BEGIN_SECTION_NO "wsActMsgBeginSectionNo"
#define WS_ACTMSG_END_SECTION_NO "wsActMsgEndSectionNo"
#define WS_ACTMSG_NOW_TIMES "wsActMsgNowTimes"
#define WS_ACTMSG_LOOP_TIMES "wsActMsgLoopTimes"
#define WS_ACTMSG_MOVE_TO_TRANS_SPEED "wsActMsgMoveToTransSpeed"
#define WS_ACTMSG_WELD_SPEED "wsActMsgWeldSpeed"
#define WS_ACTMSG_IS_CHECK "wsActMsgIsCheck"
#define WS_ACTMSG_MOD "wsActMsgMod"
#define WS_ACTMSG_LAST_MOD "wsActMsgLastMod"
#define WS_ACTMSG_USAGE "wsActMsgUsage"

//ConfigurationMsg
#define WS_CONFMSG_MOTOR_MAX_PPS "wsConfMsgMotorMaxPps"
#define WS_CONFMSG_ANGLE_MUTATION_THRESHOLD "wsConfMsgAngleMutationThreshold"
#define WS_CONFMSG_OPEN_GUN_TIME "wsConfMsgOpenGunTime"
#define WS_CONFMSG_CLOSE_GUN_TIME "wsConfMsgCloseGunTime"
#define WS_CONFMSG_TRANS_POINT_RATIO "wsConfMsgTransPointRatio"
#define WS_CONFMSG_LIMIT_POS "wsConfMsgLimitPos"
#define WS_CONFMSG_LIMIT_NEG "wsConfMsgLimitNeg"
#define WS_CONFMSG_MOTOR_RATIO "wsConfMsgMotorRatio"
#define WS_CONFMSG_MOTOR_SUBDIVISION "wsConfMsgMotorSubdivision"
#define WS_CONFMSG_ENCODER_RATIO "wsConfMsgEncoderRatio"
#define WS_CONFMSG_ENCODER_SUBDIVISION "wsConfMsgEncoderSubdivision"
#define WS_CONFMSG_PULSE_FILTER_LIMIT "wsConfMsgPulseFilterLimit"
#define WS_CONFMSG_SPEED_LIMIT "wsConfMsgSpeedLimit"
#define WS_CONFMSG_MOTOR_ACCELERATION "wsConfMsgMotorAcceleration"
#define WS_CONFMSG_KP "wsConfMsgKp"
#define WS_CONFMSG_KI "wsConfMsgKi"
#define WS_CONFMSG_KD "wsConfMsgKd"
#define WS_CONFMSG_KF "wsConfMsgKf"
#define WS_CONFMSG_KP_WORK "wsConfMsgKpWork"
#define WS_CONFMSG_KI_WORK "wsConfMsgKiWork"
#define WS_CONFMSG_KD_WORK "wsConfMsgKdWork"
#define WS_CONFMSG_KF_WORK "wsConfMsgKfWork"
#define WS_CONFMSG_N17OR5RATION "wsConfMsgN17or5ratio"
#define WS_CONFMSG_N27OR5RATION "wsConfMsgN27or5ratio"
#define WS_CONFMSG_N47OR5RATION "wsConfMsgN47or5ratio"
#define WS_CONFMSG_N67OR5RATION "wsConfMsgN67or5ratio"
#define WS_CONFMSG_N77OR5RATION "wsConfMsgN77or5ratio"
#define WS_CONFMSG_GO_HOME_LIMIT "wsConfMsgGoHomeLimit"
#define WS_CONFMSG_HOME_ANGLE "wsConfMsgHomeAngle"
#define WS_CONFMSG_GO_HOME_HIGH_SPEED "wsConfMsgGoHomeHighSpeed"
#define WS_CONFMSG_GO_HOME_LOW_SPEED "wsConfMsgGoHomeLowSpeed"
#define WS_CONFMSG_SINGLE_ABS_VALUE_AT_ORIGIN "wsConfMsgSingleAbsValueAtOrigin"
#define WS_CONFMSG_MULTI_ABS_VALUE_AT_ORIGIN "wsConfMsgMultiAbsValueAtOrigin"
#define WS_CONFMSG_MOTOR_PORT "wsConfMsgMotorPort"
#define WS_CONFMSG_ENCODER_PORT "wsConfMsgEncoderPort"
#define WS_CONFMSG_OPT_SWITCH_PORT "wsConfMsgOptSwitchPort"
#define WS_CONFMSG_MOTOR_DIRECTION "wsConfMsgMotorDirection"
#define WS_CONFMSG_ENCODER_DIRECTION "wsConfMsgEncoderDirection"
#define WS_CONFMSG_GO_HOME_DIRECTION "wsConfMsgGoHomeDirection"
#define WS_CONFMSG_ERROR_LIMIT_TRANS "wsConfMsgErrorLimitTrans"
#define WS_CONFMSG_ERROR_LIMIT_WORK "wsConfMsgErrorLimitWork"
#define WS_CONFMSG_MACHINE_PARA "wsConfMsgMachinePara"

//EditSpeedMsg
#define WS_EDITSPEEDMSG_RECORD_SIZE "wsEditSpeedMsgRecordSize"
#define WS_EDITSPEEDMSG_RECORD_FIRST "wsEditSpeedMsgRecordFirst"
#define WS_EDITSPEEDMSG_RECORD_SECOND "wsEditSpeedMsgRecordSecond"

#endif // WEBSOCKETMSGNAMES_H
