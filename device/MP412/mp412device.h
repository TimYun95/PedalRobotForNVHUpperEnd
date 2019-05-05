#ifndef MP412DEVICE_H
#define MP412DEVICE_H

#include <windows.h>

#include <QString>

class MP412Device
{
public:
    MP412Device();
    virtual ~MP412Device();

    /* 开始脉冲计数---AD读取电压---读取脉冲频率
     *  首先 StartPulseCounter() 开始脉冲计数
     *  然后 UpdateData() 读取电压和脉冲频率
     *    两个操作之间有时间间隔
     *    因为脉冲计数需要等待一定时间 利用这段时间间隔完成AD转换 */
    void StartPulseCounter(const int counterMethod);
    enum _CounterMethod{
        ForLowFreq,
        ForHighFreq
    };

    void UpdateData();

    /* 返回UpdateData()更新的数据 */
    double GetADVoltage();
    double GetPulseFrequency();

private:
    void Init();
    void Close();
    void ShowWarningBox(const QString &warningStr);

    /* 设备读取AD电压 */
    void ReadADVoltage();

    /* 设备读取脉冲频率 */
    void ReadPulseFrequency();
    double PulseFilter(const double pulseNew);//脉冲读取值的滤波
    /* 脉冲计数器和定时器的数据 转换为 脉冲频率 */
    double CounterTimerData2Frequency(const int ret, const int cntData, const int timerData);
    /* 定时器计数值 转换为 时间 */
    double TimerData2Time(const int timerData);

    /* 设置开关量输出的状态 */
    void SetDOStatus(const int pinNum, const int status);
    enum enumDOStatus{
        LowLevel=0,
        HighLevel
    };

private:
    HANDLE devHandle;

    double adVoltage;
    double pulseFrequency;
    __int32 cntPulse;

    int devStatus;//设备打开状态
    int counterStatus;//脉冲计数器和定时器的状态(是否开始工作)
    enum {
        Closed,
        Running
    };
};

#endif // MP412DEVICE_H
