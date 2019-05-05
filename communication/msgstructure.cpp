#include "msgstructure.h"

#include <cassert>

#include "msgtypes.h"
#include "common/printf.h"

using namespace MsgStructure;

/*********************纯虚基类 保存消息类型*********************/
BaseStructure::BaseStructure(const int _msgType)
    :msgType(_msgType)
{
}

BaseStructure::~BaseStructure()
{
}

int BaseStructure::GetMsgType() const
{
    return msgType;
}

void BaseStructure::SetMsgType(int type)
{
    msgType = type;
}

/**********************派生类 带消息内容***********************/
InStatus::InStatus(const int status)
    :BaseStructure(C2I_InStatus), inStatus(status)
{

}

StringStatus::StringStatus(const int status)
    :BaseStructure(C2I_StringStatus), strStatus(status)
{
}

ActionMessage::ActionMessage(ActionMsg &actMsg, const int usage)
    :BaseStructure(usage==UI2Controller? I2C_ActionStart: C2I_ActionStatus), myActMsg(&actMsg)
{
}

OutStatus::OutStatus(const int status)
    :BaseStructure(C2I_OutStatus), outStatus(status)
{
}

Configuration::Configuration(ConfigurationMsg &cm)
    :BaseStructure(I2C_Configuration), confMsg(&cm)
{
}

GoHomeResult::GoHomeResult(const int result)
    :BaseStructure(C2I_FinishGoHome), ghResult(result)
{
}

MatrixTheta::MatrixTheta(double *_theta, const int _lenTheta, double *_matrix, const int _lenMatrix)
    :BaseStructure(C2I_MatrixTheta), theta(_theta), lenTheta(_lenTheta), matrix(_matrix), lenMatrix(_lenMatrix)
{
}

EditWeldSpeed::EditWeldSpeed(const double _diffSpeed)
    :BaseStructure(I2C_EditWeldSpeed), diffSpeed(_diffSpeed)
{
}

EditSpeed::EditSpeed(EditSpeedMsg &esm)
    :BaseStructure(C2I_UpdateTeachFile), editSpeedMsg(&esm)
{
}

SingleAxis::SingleAxis(const int _axisIndex, const double _speed)
    :BaseStructure(I2C_SingleAxisSpeed0), axisIndex(_axisIndex), speed(_speed)
{
    //Modbus连续写一个寄存器 只有最后写的数据被发送
    //所以需要分离单轴的控制指令
    int singleAxisMsgType = SingleAxisIndex2MsgType(_axisIndex);
    SetMsgType(singleAxisMsgType);
}

int SingleAxis::SingleAxisIndex2MsgType(int singleAxisIndex)
{
    int singleAxisMsgType = I2C_SingleAxisSpeed0;
    switch(singleAxisIndex){
    case 0: singleAxisMsgType = I2C_SingleAxisSpeed0; break;
    case 1: singleAxisMsgType = I2C_SingleAxisSpeed1; break;
    case 2: singleAxisMsgType = I2C_SingleAxisSpeed2; break;
    case 3: singleAxisMsgType = I2C_SingleAxisSpeed3; break;
    case 4: singleAxisMsgType = I2C_SingleAxisSpeed4; break;
    case 5: singleAxisMsgType = I2C_SingleAxisSpeed5; break;
    default:
        PRINTF(LOG_ERR, "%s: invalid singleAxisIndex(%d)\n", __func__, singleAxisIndex);
        break;
    }
    return singleAxisMsgType;
}

SlowDown::SlowDown(const int _channel)
    :BaseStructure(I2C_SlowDown), channel(_channel)
{
}

OutControl::OutControl(const int status)
    :BaseStructure(I2C_OutControl), outStatus(status)
{
}

ManualControl::ManualControl(const int _method, const int _direction, const double _speed)
    :BaseStructure(I2C_ManualControl), method(_method), direction(_direction), speed(_speed)
{
}

FileContent::FileContent(const std::string &content)
    :BaseStructure(I2C_FileContent), data(content)
{
}

MonitorCommand::MonitorCommand(int *_monitorMethod, double *_monitorTheta, const int _monitorArrayLen, const int _param)
    :BaseStructure(I2C_MonitoringCommand), monitorMethod(_monitorMethod), monitorTheta(_monitorTheta), monitorArrayLen(_monitorArrayLen)
{
    params.param = _param;
}

PedalOpenValue::PedalOpenValue(const double _brakeOpenValue, const double _accOpenValue)
    :BaseStructure(I2C_PedalOpenValue), brakeOpenValue(_brakeOpenValue), accOpenValue(_accOpenValue)
{
}

MedicalForceValue::MedicalForceValue(const double _forceValue)
    :BaseStructure(I2C_ForceControl), forceValue(_forceValue)
{
}

MessageInform::MessageInform(const int _type, const double _content, const int usage)
    :BaseStructure(usage==UI2Controller? I2C_MessageInform: C2I_MessageInform), informType(_type), informContent(_content)
{
}

UnifiedInform::UnifiedInform()
    :BaseStructure(C2I_UnifiedInform), intDataArray(IntArrayLength, 0), doubleDataArray(DoubleArrayLength, 0.0)
{
}

VelocityCommand::VelocityCommand(double *_targetSpeed, const int _targetSpeedLen)
    :BaseStructure(I2C_VelocityCommand), targetSpeed(_targetSpeed), targetSpeedLen(_targetSpeedLen)
{
}
