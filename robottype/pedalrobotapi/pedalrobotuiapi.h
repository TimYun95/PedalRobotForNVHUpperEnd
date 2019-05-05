#ifndef PEDALROBOTUIAPI_H
#define PEDALROBOTUIAPI_H

#include <string>
#include <vector>

#include "UnifiedRobotApiLibrary/apidllexportdefine.h"

#ifdef ENABLE_UNIFIED_ROBOT_EXPORT_API_DLL
#include "util/highprecisiontimer.h"
#include "robottype/pedalrobot/pedalrobotui.h"
#endif

class UNIFIED_ROBOT_DLL_EXPORT PedalRobotUiApi
{
public:
    explicit PedalRobotUiApi();
    ~PedalRobotUiApi();

    enum ePedalNum{
        eBrakePedal,
        eAccPedal,

        eMaxPedalNum
    };
    enum eControlMethodType{
        eAbsolutePosition,
        eDeltaPosition,

        eMaxControlMethodType
    };
    enum eGoHomeResultType{
        eGoHomeOk,
        eGoHomeRunning,
        eGoHomeFail,

        eMaxGoHomeResultType
    };

    //是否显示UI界面
    void ShowWidget(bool flag);

    //获取CAN上的数据 踏板开度/车速/车辆模式
    double GetCanOpenValue(ePedalNum pedal);
    double GetCanCarSpeed();
    int GetCanPowermode();

    //获取CAN上的所有数据
    std::vector<double> GetCanDataAll();
    //设置CAN上的所有数据
    void SetCanDataAll(const std::vector<double> &canDatas);

    //RobotDof6PedalShiftGearSwerve设置急停的位置
    void SetEmergencyStopTheta(int emergencyStopType, const std::vector<double> &emergencyStopTheta);

    //获取MP412的脉冲车速
    double GetPulseCarSpeed();

    //机器人回原
    void DoGoHome();
    eGoHomeResultType GetGoHomeResult();

    //获取踏板机械位置
    double GetAbsolutePosition(ePedalNum pedal);
    //获取状态字符串
    std::string GetStatusString();

    //进入实时的位置控制模式 filePath指定ARM的控制文件路径
    bool EnterIntoMonitorControlMode(const std::string& actionFilePath, const std::string &curveFilePath);
    //设定实时位置控制模式的目标位置
    bool SetMonitorControlModePosition(ePedalNum pedal, double position_mm, eControlMethodType method);

    //停止机器人
    void StopRobot();

#ifdef ENABLE_UNIFIED_ROBOT_EXPORT_API_DLL
private:
    void SendMonitorCommand();

    void OnLogicControlCallbackFunc();

private:
    PedalRobotUI* m_pedalRobotUi;
    PedalRobot* m_pedalRobot;

    std::vector<double> m_monitorControlTheta;//踏板监听位置
    int m_monitorControlMethod;//踏板监听位置的方法(绝对/相对)
    bool m_enablePeriodicallySendMonitorCommand;//是否周期发送控制指令
#endif
};

#endif // PEDALROBOTUIAPI_H
