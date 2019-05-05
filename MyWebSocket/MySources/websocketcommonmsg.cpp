#include "websocketcommonmsg.h"

#include "MyWebSocket/MySources/websocketmsgnames.h"

void WebSocketCommonMsg::SerializeActMsg(const MsgStructure::ActionMessage &action, JSONWrapper &json)
{
    ActionMsg *actionMsg = action.myActMsg;

    PauseParameterMsg *pausedMsg = &(actionMsg->pauseParameter);
    json.addInt(WS_ACTMSG_IS_RESUME, pausedMsg->isResume);
    json.addDoubleArray(WS_ACTMSG_PAUSED_POSITION, pausedMsg->pausedPosition, RobotParams::axisNum);
    json.addInt(WS_ACTMSG_PAUSED_ARC_POINTS, pausedMsg->pausedArcPoints);
    json.addInt(WS_ACTMSG_PAUSED_ELLIPSE_POINTS, pausedMsg->pausedEllipsePoints);
    json.addInt(WS_ACTMSG_PAUSED_SECTIONS_NO, pausedMsg->pausedSectionSNo);
    json.addInt(WS_ACTMSG_PAUSED_INTER_POINTS, pausedMsg->pausedInterPoints);
    json.addDouble(WS_ACTMSG_PAUSED_INTERSECTION_STD_THETA, pausedMsg->intersection_std_theta);

    json.addInt(WS_ACTMSG_STATUS, actionMsg->status);
    json.addInt(WS_ACTMSG_IS_GUN_ON, actionMsg->isGunOn);
    json.addInt(WS_ACTMSG_SECTION_NO, actionMsg->sectionNo);
    json.addInt(WS_ACTMSG_PAUSED_NO, actionMsg->pausedNo);
    json.addInt(WS_ACTMSG_BEGIN_SECTION_NO, actionMsg->beginSectionNo);
    json.addInt(WS_ACTMSG_END_SECTION_NO, actionMsg->endSectionNo);
    json.addInt(WS_ACTMSG_NOW_TIMES, actionMsg->nowTimes);
    json.addInt(WS_ACTMSG_LOOP_TIMES, actionMsg->loopTimes);
    json.addDouble(WS_ACTMSG_MOVE_TO_TRANS_SPEED, actionMsg->moveToTranspositionSpeed);
    json.addDouble(WS_ACTMSG_WELD_SPEED, actionMsg->weldSpeed);
    json.addInt(WS_ACTMSG_IS_CHECK, actionMsg->isCheck);
    json.addInt(WS_ACTMSG_MOD, actionMsg->mod);
    json.addInt(WS_ACTMSG_LAST_MOD, actionMsg->lastMod);
    json.addInt(WS_ACTMSG_USAGE, actionMsg->usage);
}

void WebSocketCommonMsg::DeserializeActMsg(JSONWrapper &json, MsgStructure::ActionMessage &action)
{
    ActionMsg *actionMsg = action.myActMsg;

    PauseParameterMsg *pausedMsg = &(actionMsg->pauseParameter);
    json.loadInt(WS_ACTMSG_IS_RESUME, pausedMsg->isResume);
    json.loadDoubleArray(WS_ACTMSG_PAUSED_POSITION, pausedMsg->pausedPosition, RobotParams::axisNum);
    json.loadInt(WS_ACTMSG_PAUSED_ARC_POINTS, pausedMsg->pausedArcPoints);
    json.loadInt(WS_ACTMSG_PAUSED_ELLIPSE_POINTS, pausedMsg->pausedEllipsePoints);
    json.loadInt(WS_ACTMSG_PAUSED_SECTIONS_NO, pausedMsg->pausedSectionSNo);
    json.loadInt(WS_ACTMSG_PAUSED_INTER_POINTS, pausedMsg->pausedInterPoints);
    json.loadDouble(WS_ACTMSG_PAUSED_INTERSECTION_STD_THETA, pausedMsg->intersection_std_theta);

    json.loadInt(WS_ACTMSG_STATUS, actionMsg->status);
    json.loadInt(WS_ACTMSG_IS_GUN_ON, actionMsg->isGunOn);
    json.loadInt(WS_ACTMSG_SECTION_NO, actionMsg->sectionNo);
    json.loadInt(WS_ACTMSG_PAUSED_NO, actionMsg->pausedNo);
    json.loadInt(WS_ACTMSG_BEGIN_SECTION_NO, actionMsg->beginSectionNo);
    json.loadInt(WS_ACTMSG_END_SECTION_NO, actionMsg->endSectionNo);
    json.loadInt(WS_ACTMSG_NOW_TIMES, actionMsg->nowTimes);
    json.loadInt(WS_ACTMSG_LOOP_TIMES, actionMsg->loopTimes);
    json.loadDouble(WS_ACTMSG_MOVE_TO_TRANS_SPEED, actionMsg->moveToTranspositionSpeed);
    json.loadDouble(WS_ACTMSG_WELD_SPEED, actionMsg->weldSpeed);
    json.loadInt(WS_ACTMSG_IS_CHECK, actionMsg->isCheck);
    json.loadInt(WS_ACTMSG_MOD, actionMsg->mod);
    json.loadInt(WS_ACTMSG_LAST_MOD, actionMsg->lastMod);
    json.loadInt(WS_ACTMSG_USAGE, actionMsg->usage);
}

void WebSocketCommonMsg::SerializeConfMsg(const MsgStructure::Configuration &conf, JSONWrapper &json)
{
    ConfigurationMsg *confMsg = conf.confMsg;

    json.addInt(WS_CONFMSG_MOTOR_MAX_PPS, confMsg->motorMaxPps);
    json.addInt(WS_CONFMSG_ANGLE_MUTATION_THRESHOLD, confMsg->angleMutationThreshold);
    json.addInt(WS_CONFMSG_OPEN_GUN_TIME, confMsg->openGunTime);
    json.addInt(WS_CONFMSG_CLOSE_GUN_TIME, confMsg->closeGunTime);
    json.addInt(WS_CONFMSG_TRANS_POINT_RATIO, confMsg->transPointRatio);

    json.addDoubleArray(WS_CONFMSG_LIMIT_POS, confMsg->limitPos, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_LIMIT_NEG, confMsg->limitNeg, RobotParams::axisNum);

    json.addDoubleArray(WS_CONFMSG_MOTOR_RATIO, confMsg->motorRatio, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_MOTOR_SUBDIVISION, confMsg->motorSubdivision, RobotParams::axisNum);

    json.addDoubleArray(WS_CONFMSG_ENCODER_RATIO, confMsg->encoderRatio, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_ENCODER_SUBDIVISION, confMsg->encoderSubdivision, RobotParams::axisNum);

    json.addIntArray(WS_CONFMSG_PULSE_FILTER_LIMIT, confMsg->pulse_filter_limit, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_SPEED_LIMIT, confMsg->speedLimit, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_MOTOR_ACCELERATION, confMsg->motorAcceleration, RobotParams::axisNum);

    json.addDoubleArray(WS_CONFMSG_KP, confMsg->kp, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_KI, confMsg->ki, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_KD, confMsg->kd, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_KF, confMsg->kf, RobotParams::axisNum);

    json.addDoubleArray(WS_CONFMSG_KP_WORK, confMsg->kp_work, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_KI_WORK, confMsg->ki_work, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_KD_WORK, confMsg->kd_work, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_KF_WORK, confMsg->kf_work, RobotParams::axisNum);

    json.addDouble(WS_CONFMSG_N17OR5RATION, confMsg->N1_7or5_ratio);
    json.addDouble(WS_CONFMSG_N27OR5RATION, confMsg->N2_7or5_ratio);
    json.addDouble(WS_CONFMSG_N47OR5RATION, confMsg->N4_7or5_ratio);
    json.addDouble(WS_CONFMSG_N67OR5RATION, confMsg->N6_7or5_ratio);
    json.addDouble(WS_CONFMSG_N77OR5RATION, confMsg->N7_7or5_ratio);

    json.addDoubleArray(WS_CONFMSG_GO_HOME_LIMIT, confMsg->goHomeLimit, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_HOME_ANGLE, confMsg->homeAngle, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_GO_HOME_HIGH_SPEED, confMsg->goHomeHighSpeed, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_GO_HOME_LOW_SPEED, confMsg->goHomeLowSpeed, RobotParams::axisNum);

    json.addIntArray(WS_CONFMSG_SINGLE_ABS_VALUE_AT_ORIGIN, confMsg->singleAbsValueAtOrigin, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_MULTI_ABS_VALUE_AT_ORIGIN, confMsg->multiAbsValueAtOrigin, RobotParams::axisNum);

    json.addIntArray(WS_CONFMSG_MOTOR_PORT, confMsg->motorPort, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_ENCODER_PORT, confMsg->encoderPort, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_OPT_SWITCH_PORT, confMsg->optSwitchPort, RobotParams::axisNum);

    json.addIntArray(WS_CONFMSG_MOTOR_DIRECTION, confMsg->motorDirection, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_ENCODER_DIRECTION, confMsg->encoderDirection, RobotParams::axisNum);
    json.addIntArray(WS_CONFMSG_GO_HOME_DIRECTION, confMsg->goHomeDirection, RobotParams::axisNum);

    json.addDoubleArray(WS_CONFMSG_ERROR_LIMIT_TRANS, confMsg->errorLimitTrans, RobotParams::axisNum);
    json.addDoubleArray(WS_CONFMSG_ERROR_LIMIT_WORK, confMsg->errorLimitWork, RobotParams::dof);

    json.addDoubleArray(WS_CONFMSG_MACHINE_PARA, confMsg->machinePara, RobotParams::machineParamNum);
}

void WebSocketCommonMsg::DeserializeConfMsg(JSONWrapper &json, MsgStructure::Configuration &conf)
{
    ConfigurationMsg *confMsg = conf.confMsg;

    json.loadInt(WS_CONFMSG_MOTOR_MAX_PPS, confMsg->motorMaxPps);
    json.loadInt(WS_CONFMSG_ANGLE_MUTATION_THRESHOLD, confMsg->angleMutationThreshold);
    json.loadInt(WS_CONFMSG_OPEN_GUN_TIME, confMsg->openGunTime);
    json.loadInt(WS_CONFMSG_CLOSE_GUN_TIME, confMsg->closeGunTime);
    json.loadInt(WS_CONFMSG_TRANS_POINT_RATIO, confMsg->transPointRatio);

    json.loadDoubleArray(WS_CONFMSG_LIMIT_POS, confMsg->limitPos, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_LIMIT_NEG, confMsg->limitNeg, RobotParams::axisNum);

    json.loadDoubleArray(WS_CONFMSG_MOTOR_RATIO, confMsg->motorRatio, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_MOTOR_SUBDIVISION, confMsg->motorSubdivision, RobotParams::axisNum);

    json.loadDoubleArray(WS_CONFMSG_ENCODER_RATIO, confMsg->encoderRatio, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_ENCODER_SUBDIVISION, confMsg->encoderSubdivision, RobotParams::axisNum);

    json.loadIntArray(WS_CONFMSG_PULSE_FILTER_LIMIT, confMsg->pulse_filter_limit, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_SPEED_LIMIT, confMsg->speedLimit, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_MOTOR_ACCELERATION, confMsg->motorAcceleration, RobotParams::axisNum);

    json.loadDoubleArray(WS_CONFMSG_KP, confMsg->kp, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_KI, confMsg->ki, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_KD, confMsg->kd, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_KF, confMsg->kf, RobotParams::axisNum);

    json.loadDoubleArray(WS_CONFMSG_KP_WORK, confMsg->kp_work, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_KI_WORK, confMsg->ki_work, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_KD_WORK, confMsg->kd_work, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_KF_WORK, confMsg->kf_work, RobotParams::axisNum);

    json.loadDouble(WS_CONFMSG_N17OR5RATION, confMsg->N1_7or5_ratio);
    json.loadDouble(WS_CONFMSG_N27OR5RATION, confMsg->N2_7or5_ratio);
    json.loadDouble(WS_CONFMSG_N47OR5RATION, confMsg->N4_7or5_ratio);
    json.loadDouble(WS_CONFMSG_N67OR5RATION, confMsg->N6_7or5_ratio);
    json.loadDouble(WS_CONFMSG_N77OR5RATION, confMsg->N7_7or5_ratio);

    json.loadDoubleArray(WS_CONFMSG_GO_HOME_LIMIT, confMsg->goHomeLimit, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_HOME_ANGLE, confMsg->homeAngle, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_GO_HOME_HIGH_SPEED, confMsg->goHomeHighSpeed, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_GO_HOME_LOW_SPEED, confMsg->goHomeLowSpeed, RobotParams::axisNum);

    json.loadIntArray(WS_CONFMSG_SINGLE_ABS_VALUE_AT_ORIGIN, confMsg->singleAbsValueAtOrigin, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_MULTI_ABS_VALUE_AT_ORIGIN, confMsg->multiAbsValueAtOrigin, RobotParams::axisNum);

    json.loadIntArray(WS_CONFMSG_MOTOR_PORT, confMsg->motorPort, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_ENCODER_PORT, confMsg->encoderPort, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_OPT_SWITCH_PORT, confMsg->optSwitchPort, RobotParams::axisNum);

    json.loadIntArray(WS_CONFMSG_MOTOR_DIRECTION, confMsg->motorDirection, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_ENCODER_DIRECTION, confMsg->encoderDirection, RobotParams::axisNum);
    json.loadIntArray(WS_CONFMSG_GO_HOME_DIRECTION, confMsg->goHomeDirection, RobotParams::axisNum);

    json.loadDoubleArray(WS_CONFMSG_ERROR_LIMIT_TRANS, confMsg->errorLimitTrans, RobotParams::axisNum);
    json.loadDoubleArray(WS_CONFMSG_ERROR_LIMIT_WORK, confMsg->errorLimitWork, RobotParams::dof);

    json.loadDoubleArray(WS_CONFMSG_MACHINE_PARA, confMsg->machinePara, RobotParams::machineParamNum);
}

void WebSocketCommonMsg::SerializeEditSpeedMsg(const MsgStructure::EditSpeed &edit, JSONWrapper &json)
{
    EditSpeedMsg *editSpeedMsg = edit.editSpeedMsg;

    int len = editSpeedMsg->record.size();
    int recordFirst[len];
    double recordSecond[len];
    for(int i=0; i<len; ++i){
        recordFirst[i] = editSpeedMsg->record[i].first;//int
        recordSecond[i] = editSpeedMsg->record[i].second;//double
    }

    json.addInt(WS_EDITSPEEDMSG_RECORD_SIZE, len);
    json.addIntArray(WS_EDITSPEEDMSG_RECORD_FIRST, recordFirst, len);
    json.addDoubleArray(WS_EDITSPEEDMSG_RECORD_SECOND, recordSecond, len);
}

void WebSocketCommonMsg::DeserializeEditSpeedMsg(JSONWrapper &json, MsgStructure::EditSpeed &edit)
{
    EditSpeedMsg *editSpeedMsg = edit.editSpeedMsg;

    int len;
    json.loadInt(WS_EDITSPEEDMSG_RECORD_SIZE, len);
    int recordFirst[len];
    double recordSecond[len];
    json.loadIntArray(WS_EDITSPEEDMSG_RECORD_FIRST, recordFirst, len);
    json.loadDoubleArray(WS_EDITSPEEDMSG_RECORD_SECOND, recordSecond, len);

    editSpeedMsg->record.clear();
    editSpeedMsg->record.reserve(len);
    for(int i=0; i<len; ++i){
        editSpeedMsg->record.push_back( std::make_pair(recordFirst[i], recordSecond[i]) );
    }
}
