#ifndef ROBOTPARAMS_H
#define ROBOTPARAMS_H

/* URITimerMs*URITimerMultiplier=实际周期
 * QT中20ms以内的定时器 高精度
 * 定时器回调函数中软件倍频 */

class RobotParams
{
public:
    //UI界面
    //统一6轴6自由度布局控件
    static const int UIAxisNum=6;
    static const int UIdof=6;
    //统一3*7组的布局控件
    static const int machineParamNum=21;
    //预留的备用参数(不同类型的机器人具有不同的用途)
    static const int reservedParamNum=50;

#if defined DOF_4_MEDICAL
    static const int logicUpdateTimer_IntervalMs=10;//10ms
    static const int logicUpdateTimer_IntervalMultiplier=1;
    static const int widgetUpdateTimer_IntervalMs=200;//200ms
    static const int widgetUpdateTimer_IntervalMultiplier=1;
    static const int axisNum=6;
    static const int dof=6;
#elif defined(DOF_2_PEDAL) || defined(DOF_3_PEDAL_API)
    static const int logicUpdateTimer_IntervalMs=18;//54ms
    static const int logicUpdateTimer_IntervalMultiplier=3;
    static const int widgetUpdateTimer_IntervalMs=54;//216ms
    static const int widgetUpdateTimer_IntervalMultiplier=4;
    static const int axisNum=6;
    static const int dof=6;
#elif defined DOF_5_SCARA_PAINT
    static const int logicUpdateTimer_IntervalMs=80;//80ms
    static const int logicUpdateTimer_IntervalMultiplier=1;
    static const int widgetUpdateTimer_IntervalMs=80;//80ms
    static const int widgetUpdateTimer_IntervalMultiplier=1;
    static const int axisNum=5;
    static const int dof=5;
#elif defined DOF_5_IGUS_FLEXIBLE_SHAFT
    static const int logicUpdateTimer_IntervalMs=80;//80ms
    static const int logicUpdateTimer_IntervalMultiplier=1;
    static const int widgetUpdateTimer_IntervalMs=80;//80ms
    static const int widgetUpdateTimer_IntervalMultiplier=1;
    static const int axisNum=5;
    static const int dof=5;
#elif defined DOF_6_XYZ_VESSEL_PUNCTURE
    static const int logicUpdateTimer_IntervalMs=80;//80ms
    static const int logicUpdateTimer_IntervalMultiplier=1;
    static const int widgetUpdateTimer_IntervalMs=80;//80ms
    static const int widgetUpdateTimer_IntervalMultiplier=1;
    static const int axisNum=6;
    static const int dof=6;

#endif

    static const char* robotType;
    static const char* robotFolder;
};

#endif // ROBOTPARAMS_H
