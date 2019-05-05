#ifndef MSGSTRUCTURE_H
#define MSGSTRUCTURE_H

#include "common/message.h"

/* 各种不同的消息结构体 */
namespace MsgStructure {

/* 指定消息的去向(有些消息结构体是双向通讯的) */
enum tagMsgUsage{
    UI2Controller,
    Controller2UI
};

/*********************纯虚基类 保存消息类型*********************/
struct BaseStructure
{
public:
    BaseStructure(const int _msgType);
    virtual ~BaseStructure()=0;

    int GetMsgType() const;
    void SetMsgType(int type);

protected:
    int msgType;
};

/**********************派生类 带消息内容***********************/
struct InStatus: public BaseStructure
{
public:
    InStatus(const int status=0);
    int inStatus;
};

struct OutStatus: public BaseStructure
{
public:
    OutStatus(const int status=0);
    int outStatus;
};

struct OutControl: public BaseStructure
{
public:
    OutControl(const int status=0);
    int outStatus;
};

struct StringStatus: public BaseStructure
{
public:
    StringStatus(const int status=0);
    int strStatus;
};

struct ActionMessage: public BaseStructure
{
public:
    ActionMessage(ActionMsg &actMsg, const int usage);
    ActionMsg* myActMsg;
};

struct EditSpeed: public BaseStructure
{
public:
    EditSpeed(EditSpeedMsg &esm);
    EditSpeedMsg *editSpeedMsg;
};

struct Configuration: public BaseStructure
{
public:
    Configuration(ConfigurationMsg &cm);
    ConfigurationMsg *confMsg;
};

struct GoHomeResult: public BaseStructure
{
public:
    GoHomeResult(const int result=0);
    int ghResult;
    enum tagGoHomeResult{
        Fail=0,
        Success,
        RequestTeachFile,//请求UI发送示教文件，运动到回原后的指定点
        MovingToTargetPoint,//回原后移动到指定点 过程中
        MovingToTargetPointSuccessfully,//回原后移动到指定点 成功
        MovingToTargetPointUnsuccessfully//回原后移动到指定点 失败
    };
};

struct MatrixTheta: public BaseStructure
{
public:
    MatrixTheta(double *_theta, const int _lenTheta, double *_matrix, const int _lenMatrix);
    double *theta;
    int lenTheta;
    double *matrix;
    int lenMatrix;
};

struct EditWeldSpeed: public BaseStructure
{
public:
    EditWeldSpeed(const double _diffSpeed=0.0);
    double diffSpeed;
};

struct SlowDown: public BaseStructure
{
public:
    SlowDown(const int _channel=1);
    int channel;
};

struct SingleAxis: public BaseStructure
{
public:
    SingleAxis(const int _axisIndex=0, const double _speed=0.0);
    int axisIndex;
    double speed;

    static int SingleAxisIndex2MsgType(int singleAxisIndex);
};

struct ManualControl: public BaseStructure
{
public:
    ManualControl(const int _method=0, const int _direction=0, const double _speed=0.0);
    int method;
    int direction;
    double speed;
};

struct FileContent: public BaseStructure
{
public:
    explicit FileContent(const std::string& content);
    std::string data;
private:
    FileContent();//接收消息时 请给content随意赋值 拒绝默认构造函数
};

struct MonitorCommand: public BaseStructure
{
public:
    //_controlMethod和_theta相同的长度_thetaLen
    MonitorCommand(int *_monitorMethod=NULL, double* _monitorTheta=NULL, const int _monitorArrayLen=0, const int _param=0);

    enum tagControlMethod{
        DeltaControl=0,//PedalRobot
        ManualAutoControl,//PedalRobot
        ManualEncoderControl,//PedalRobot
        AbsControl,//MedicalUltrasoundRobot

        MaxControlMethod
    };

    int *monitorMethod;
    double* monitorTheta;
    int monitorArrayLen;

    union tagParams{
        int param;//消息通讯时使用
        int timeStamp;//PedalRobot
        int movingAxis;//MedicalUltrasoundRobot
    }params;//该变量对不同的机器人有不同的意义
};

struct PedalOpenValue: public BaseStructure
{
public:
    PedalOpenValue(const double _brakeOpenValue=0.0, const double _accOpenValue=0.0);
    double brakeOpenValue;
    double accOpenValue;
};

struct MedicalForceValue: public BaseStructure
{
public:
    MedicalForceValue(const double _forceValue=0.0);
    double forceValue;
};

struct MessageInform: public BaseStructure
{
public:
    enum TagInformType{
        NullType=0,
        AtSuspendPoint,
        ExitWaitPoint,
        ShowAlarm,
        FinishAction,
        EnterIdleState,
        UriIsStarted,
        GoHome,
        PauseAction,
        ResumeAction,
        ShutDown,
        Echo,
        ReInit,
        MedicalAxis4Angle,
        EmergencyStop,
        EmergencyStopError,
        SaveLogger,
        EnableLogger,
        TagInformTypeLength
    };

    MessageInform(const int _type, const double _content, const int usage);
    int informType;
    double informContent;
};

struct UnifiedInform: public BaseStructure
{
public:
    UnifiedInform();

    enum DataArrayLength{
        IntArrayLength = 10,
        DoubleArrayLength = 10
    };

    std::vector<int> intDataArray;
    std::vector<double> doubleDataArray;
};

struct VelocityCommand: public BaseStructure
{
public:
    VelocityCommand(double* _targetSpeed=NULL, const int _targetSpeedLen=0);

    double* targetSpeed;
    int targetSpeedLen;
};

}

#endif // MSGSTRUCTURE_H
