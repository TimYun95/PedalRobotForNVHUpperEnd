#include "message.h"

bool ActionMsg::operator ==(const ActionMsg actMsg) const
{
    if(this->status != actMsg.status ||
            this->isGunOn      != actMsg.isGunOn ||
            this->sectionNo    != actMsg.sectionNo ||
            this->pausedNo     != actMsg.pausedNo ||
            this->beginSectionNo != actMsg.beginSectionNo ||
            this->endSectionNo != actMsg.endSectionNo ||
            this->nowTimes     != actMsg.nowTimes ||
            this->loopTimes    != actMsg.loopTimes ||
            this->moveToTranspositionSpeed != actMsg.moveToTranspositionSpeed ||
            this->weldSpeed    != actMsg.weldSpeed ||
            this->isCheck      != actMsg.isCheck ||
            this->mod          != actMsg.mod ||
            this->lastMod      != actMsg.lastMod ||
            this->usage        != actMsg.usage){
        return false;
    }else{
        return true;
    }
}

std::tr1::shared_ptr<EndianConverter> ActionMsg::Serialize() const
{
    std::tr1::shared_ptr<EndianConverter> ec(new EndianConverter());
    Construct( *(ec.get()) );

    return ec;
}

void ActionMsg::Deserialize(unsigned short *addr)
{
    EndianConverter ec;
    Construct(ec);
    ec.loadFromRegisters(addr, ec.length()/2);
    ec.toLocal();

    pauseParameter.isResume=ec.parseShort();
    for(int i=0;i<RobotParams::axisNum;i++){
        pauseParameter.pausedPosition[i]=ec.parseDouble();
    }
    pauseParameter.pausedArcPoints=ec.parseInt();
    pauseParameter.pausedEllipsePoints=ec.parseInt();
    pauseParameter.pausedSectionSNo=ec.parseInt();
    pauseParameter.pausedInterPoints=ec.parseInt();
    pauseParameter.intersection_std_theta=ec.parseDouble();

    status=ec.parseInt();
    isGunOn=ec.parseShort();
    sectionNo=ec.parseInt();
    pausedNo=ec.parseInt();
    beginSectionNo=ec.parseInt();
    endSectionNo=ec.parseInt();
    nowTimes=ec.parseInt();
    loopTimes=ec.parseInt();
    moveToTranspositionSpeed=ec.parseDouble();
    weldSpeed=ec.parseDouble();
    isCheck=ec.parseShort();
    mod=ec.parseShort();
    lastMod=ec.parseShort();
    usage=ec.parseShort();
}

void ActionMsg::Construct(EndianConverter &ec) const
{
    ec.addShort( pauseParameter.isResume );
    ec.addDoubleArray( pauseParameter.pausedPosition );
    ec.addInt( pauseParameter.pausedArcPoints );
    ec.addInt( pauseParameter.pausedEllipsePoints );
    ec.addInt( pauseParameter.pausedSectionSNo );
    ec.addInt( pauseParameter.pausedInterPoints );
    ec.addDouble( pauseParameter.intersection_std_theta );

    ec.addInt( status );
    ec.addShort( isGunOn );
    ec.addInt( sectionNo );
    ec.addInt( pausedNo );
    ec.addInt( beginSectionNo );
    ec.addInt( endSectionNo );
    ec.addInt( nowTimes );
    ec.addInt( loopTimes );
    ec.addDouble( moveToTranspositionSpeed );
    ec.addDouble( weldSpeed );
    ec.addShort( isCheck );
    ec.addShort( mod );
    ec.addShort( lastMod );
    ec.addShort( usage );
}

std::tr1::shared_ptr<EndianConverter> ConfigurationMsg::Serialize() const
{
    std::tr1::shared_ptr<EndianConverter> ec(new EndianConverter());
    Construct( *(ec.get()) );

    return ec;
}

void ConfigurationMsg::Deserialize(unsigned short* addr)
{
    EndianConverter ec;
    Construct(ec);
    ec.loadFromRegisters(addr, ec.length()/2);
    ec.toLocal();

    motorMaxPps = ec.parseInt();
    angleMutationThreshold = ec.parseInt();
    openGunTime = ec.parseInt();
    closeGunTime = ec.parseInt();
    transPointRatio = ec.parseInt();

    for(int i=0; i<RobotParams::axisNum; ++i) limitPos[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) limitNeg[i] = ec.parseDouble();

    for(int i=0; i<RobotParams::axisNum; ++i) motorRatio[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) motorSubdivision[i] = ec.parseInt();

    for(int i=0; i<RobotParams::axisNum; ++i) encoderRatio[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) encoderSubdivision[i] = ec.parseInt();

    for(int i=0; i<RobotParams::axisNum; ++i) pulse_filter_limit[i] = ec.parseInt();
    for(int i=0; i<RobotParams::axisNum; ++i) speedLimit[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) motorAcceleration[i] = ec.parseDouble();

    for(int i=0; i<RobotParams::axisNum; ++i) maxMonitorDiffTheta[i] = ec.parseDouble();

    for(int i=0; i<RobotParams::axisNum; ++i) kp[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) ki[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) kd[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) kf[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) kp_work[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) ki_work[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) kd_work[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) kf_work[i] = ec.parseDouble();

    N1_7or5_ratio = ec.parseDouble();
    N2_7or5_ratio = ec.parseDouble();
    N4_7or5_ratio = ec.parseDouble();
    N6_7or5_ratio = ec.parseDouble();
    N7_7or5_ratio = ec.parseDouble();

    for(int i=0; i<RobotParams::axisNum; ++i) goHomeLimit[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) homeAngle[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) goHomeHighSpeed[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::axisNum; ++i) goHomeLowSpeed[i] = ec.parseDouble();

    for(int i=0; i<RobotParams::axisNum; ++i) singleAbsValueAtOrigin[i] = ec.parseInt();
    for(int i=0; i<RobotParams::axisNum; ++i) multiAbsValueAtOrigin[i] = ec.parseInt();

    for(int i=0; i<RobotParams::axisNum; ++i) motorPort[i] = ec.parseInt();
    for(int i=0; i<RobotParams::axisNum; ++i) encoderPort[i] = ec.parseInt();
    for(int i=0; i<RobotParams::axisNum; ++i) optSwitchPort[i] = ec.parseInt();

    for(int i=0; i<RobotParams::axisNum; ++i) motorDirection[i] = ec.parseInt();
    for(int i=0; i<RobotParams::axisNum; ++i) encoderDirection[i] = ec.parseInt();
    for(int i=0; i<RobotParams::axisNum; ++i) goHomeDirection[i] = ec.parseInt();

    for(int i=0; i<RobotParams::axisNum; ++i) errorLimitTrans[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::dof; ++i) errorLimitWork[i] = ec.parseDouble();

    for(int i=0; i<RobotParams::machineParamNum; ++i) machinePara[i] = ec.parseDouble();
    for(int i=0; i<RobotParams::reservedParamNum; ++i) reservedParam[i] = ec.parseDouble();
}

void ConfigurationMsg::Construct(EndianConverter &ec) const
{
    ec.addInt( motorMaxPps );
    ec.addInt( angleMutationThreshold );
    ec.addInt( openGunTime );
    ec.addInt( closeGunTime );
    ec.addInt( transPointRatio );

    ec.addDoubleArray( limitPos);
    ec.addDoubleArray( limitNeg );

    ec.addDoubleArray( motorRatio );
    ec.addIntArray( motorSubdivision );

    ec.addDoubleArray( encoderRatio );
    ec.addIntArray( encoderSubdivision);

    ec.addIntArray( pulse_filter_limit );
    ec.addDoubleArray( speedLimit );
    ec.addDoubleArray( motorAcceleration );

    ec.addDoubleArray( maxMonitorDiffTheta );

    ec.addDoubleArray( kp );
    ec.addDoubleArray( ki );
    ec.addDoubleArray( kd );
    ec.addDoubleArray( kf );
    ec.addDoubleArray( kp_work );
    ec.addDoubleArray( ki_work );
    ec.addDoubleArray( kd_work );
    ec.addDoubleArray( kf_work );

    ec.addDouble( N1_7or5_ratio );
    ec.addDouble( N2_7or5_ratio );
    ec.addDouble( N4_7or5_ratio );
    ec.addDouble( N6_7or5_ratio );
    ec.addDouble( N7_7or5_ratio );

    ec.addDoubleArray( goHomeLimit );
    ec.addDoubleArray( homeAngle );
    ec.addDoubleArray( goHomeHighSpeed );
    ec.addDoubleArray( goHomeLowSpeed );

    ec.addIntArray( singleAbsValueAtOrigin );
    ec.addIntArray( multiAbsValueAtOrigin );

    ec.addIntArray( motorPort );
    ec.addIntArray( encoderPort );
    ec.addIntArray( optSwitchPort );

    ec.addIntArray( motorDirection );
    ec.addIntArray( encoderDirection );
    ec.addIntArray( goHomeDirection );

    ec.addDoubleArray( errorLimitTrans );
    ec.addDoubleArray( errorLimitWork );

    ec.addDoubleArray( machinePara );
    ec.addDoubleArray( reservedParam );
}

std::tr1::shared_ptr<EndianConverter> EditSpeedMsg::Serialize() const
{
    std::tr1::shared_ptr<EndianConverter> ec(new EndianConverter());
    Construct( *(ec.get()) );

    return ec;
}

void EditSpeedMsg::Deserialize(unsigned short *addr)
{
    EndianConverter ec;
    Construct(ec);
    ec.loadFromRegisters(addr, ec.length()/2);
    ec.toLocal();

    //首先获取长度
    int len = ec.parseInt();
    record.resize(len);
    for(int i=0; i<len; ++i) record[i].first = ec.parseInt();
    for(int i=0; i<len; ++i) record[i].second = ec.parseDouble();
}

void EditSpeedMsg::Construct(EndianConverter &ec) const
{
    size_t len = record.size();
    ec.addInt( len );
    for(size_t i=0; i<len; ++i) ec.addInt( record[i].first );
    for(size_t i=0; i<len; ++i) ec.addDouble( record[i].second );
}
