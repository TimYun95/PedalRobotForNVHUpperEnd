#include "mp412device.h"

#include <sys/time.h>

#include <iostream>

#include <QMessageBox>

#include "mp412.h"
#include "common/printf.h"

//设备号=0(第一个MP412设备)
const __int32 devNum = 0;

//AD转换(仪器输出的电压不稳定 舍弃该方案)
const __int32 adChannel=15;//通道15(连接PIN16-17)
const __int32 adGain   =0;//增益量程0-10V
const __int32 adSidi   =0;//单端输入
const __int32 adNaver  =1;//采样的平均次数
//脉冲计数
/* 脉冲计数器=减法计数器(从cntPulse开始) 定时计数器=加法计数器(从0开始) */
const __int32 cntChannel= 1;//通道1(连接PIN18-19)
//24位计数器初始化数据 (计数多少个脉冲上升沿 必须>1)
const __int32 cntPulseForHighFreq  = 10;//高频脉冲 测多个脉冲的时间宽度
const __int32 cntPulseForLowFreq = 2;//低频脉冲 测2个脉冲间的时间宽度

MP412Device::MP412Device()
{
    devHandle = NULL;
    devStatus = Closed;

    adVoltage = 0.0;
    pulseFrequency = 0.0;

    Init();
}

MP412Device::~MP412Device()
{
    Close();
}

void MP412Device::UpdateData()
{
    //先读取电压 这段时间可以提供给MP412测脉冲
    //ReadADVoltage();//AD目前不需要了

    //再去读取脉冲数据
    ReadPulseFrequency();
}

double MP412Device::GetADVoltage()
{
    return adVoltage;
}

double MP412Device::GetPulseFrequency()
{
    return pulseFrequency;
}

void MP412Device::Init()
{
    if(devHandle != NULL){
        PRINTF(LOG_ERR, "%s: MP412 has already been opened!\n", __func__);
        return;
    }

    //open
    devHandle = MP412_OpenDevice(devNum);
    if(devHandle == INVALID_HANDLE_VALUE){
        PRINTF(LOG_ERR, "%s: cannot open MP412 device!\n", __func__);
        ShowWarningBox("Cannot open MP412 device!");
        return;
    }
    PRINTF(LOG_DEBUG, "%s: MP412 open...OK!\n", __func__);

    //calibrate(在上电后第一次采样之前必须调用一次cal操作, 否则采样误差极大)
    __int32 ret = MP412_CAL(devHandle);
    if(ret != 0){
        PRINTF(LOG_ERR, "%s: MP412 AD calibrate error!\n", __func__);
        ShowWarningBox("MP412 AD calibrate error!");
        return;
    }
    PRINTF(LOG_DEBUG, "%s: MP412 AD calibrate...OK!\n", __func__);

    devStatus = Running;
}

void MP412Device::Close()
{
    if(devHandle == NULL){
        PRINTF(LOG_ERR, "%s: MP412 has already been closed!\n", __func__);
        return;
    }

    const __int32 ret = MP412_CloseDevice(devHandle);
    if(ret == -1){
        PRINTF(LOG_ERR, "%s: cannot close MP412 device!\n", __func__);
        return;
    }
    PRINTF(LOG_DEBUG, "%s: MP412 close...OK\n", __func__);

    devHandle = NULL;
    devStatus = Closed;
}

void MP412Device::ShowWarningBox(const QString &warningStr)
{
#ifdef ENABLE_MP412_WARNINGBOX
    QMessageBox::information(NULL, "warning", warningStr);
#else
    Q_UNUSED(warningStr);
#endif
}

void MP412Device::ReadADVoltage()
{
    if(devStatus != Running){
        adVoltage = -1.0;
#ifdef ENABLE_DEVICE_ERROR_DEBUG
        PRINTF(LOG_ERR, "%s: MP412 is not opened yet.\n", __func__);
#endif
        return;
    }

    //AD查询模式 单次采样约耗时0.5ms
    //adData = 12位采样数据 = 0~4095
    const __int32 adData = MP412_AD(devHandle, adChannel, adGain, adSidi, adNaver);

    //计算电压
    const double voltage_mV = MP412_ADV(adGain, adData);
    //std::cout<<"voltage="<<voltage_mV<<"mV"<<std::endl;
    adVoltage = voltage_mV/1000.0;
}

void MP412Device::StartPulseCounter(const int counterMethod)
{
    if(devStatus != Running){
        pulseFrequency = -1.0;
#ifdef ENABLE_DEVICE_ERROR_DEBUG
        PRINTF(LOG_ERR, "%s: MP412 is not opened yet.\n", __func__);
#endif
        return;
    }

    counterStatus = Closed;

    //启动脉冲计数相关的计数器/定时器
    cntPulse = counterMethod == ForLowFreq? cntPulseForLowFreq: cntPulseForHighFreq;
    const __int32 ret = MP412_CNT_Run(devHandle, cntChannel, cntPulse);
    if(ret != 0){
        PRINTF(LOG_ERR, "%s: MP412 start the pulse counter error!\n", __func__);
        return;
    }

    counterStatus = Running;
}

void MP412Device::ReadPulseFrequency()
{
    if(devStatus != Running){
        pulseFrequency = -1.0;
#ifdef ENABLE_DEVICE_ERROR_DEBUG
        PRINTF(LOG_ERR, "%s: MP412 is not opened yet.\n", __func__);
#endif
        return;
    }

    if(counterStatus != Running){
        pulseFrequency = -2.0;
        PRINTF(LOG_ERR, "%s: counter/timer is not started.\n", __func__);
        return;
    }else
    counterStatus = Closed;

    //读取计数器/定时器数据
    __int32 cntData;
    __int32 timerData;
    __int32 ret = MP412_CNT_Read(devHandle, cntChannel, &cntData, &timerData);
    //std::cout<<"cnt="<<cntData<<" timer="<<timerData<<std::endl;

    const double pulseRead = CounterTimerData2Frequency(ret, cntData, timerData);
    //std::cout<<"Frequency="<<pulseFrequency<<"Hz"<<std::endl;

    pulseFrequency = PulseFilter(pulseRead);
}

double MP412Device::PulseFilter(const double pulseNew)
{
    //回环队列:新数据 替换 最旧的数据
    static const int length = 8;
    static int index = -1;
    static double record[length]={0.0};
    index = (index+1)%length;
    record[index] = pulseNew;

    //去除最大最小后求平均
    double sum = 0.0;
    double maxRecord = record[0];
    double minRecord = record[0];
    for(int i=0; i<length; ++i){
        sum += record[i];
        if(maxRecord < record[i]){
            maxRecord = record[i];
        }
        if(minRecord > record[i]){
            minRecord = record[i];
        }
    }
    return (sum-maxRecord-minRecord) / (length-2);
}

double MP412Device::CounterTimerData2Frequency(const int ret, const int cntData, const int timerData)
{
    /* 因为 "在第一个有效输入脉冲边沿到来时开始定时器的加法计数"
     * 所以 定时器计数时间内 有XXX-1个脉冲 */
    double pulseNum;//脉冲个数
    if(ret == 1){//计数器减法0溢出(定时器会保留数据)
        pulseNum = cntPulse - 0 - 1;
        /* 该情况适用于cntPulse=5(较小的值) 然后延时很短的时间(eg. 1ms) 读取计数器和定时器的数据
         * 本程序当前使用该方法 这段很短的时间内,完成CAN的读取以及AD的读取
         * 若这段时间太短 可以加大adNaver来增长时间 */
    }else if(timerData == 0xFFFFFF){//定时器超量程
        pulseNum = -1.0;
        PRINTF(LOG_ERR, "%s: timer overflow! This should never happen!\n", __func__);
    }else{//正常情况
        pulseNum = cntPulse - cntData - 1;//脉冲减法计数器获得该段时间内的脉冲个数
        /* 该情况适用于cntPulse=0xFFFFFF 然后延时几十ms后 读取计数器和定时器的数据 */
    }

    const double pulseTime = TimerData2Time(timerData);//脉冲宽度的时间

    //pulseTime(timerData)=0 没有脉冲到达(因为第一个脉冲后开始定时器) 即频率=0
    double frequency = 0.0;
    if(pulseTime != 0.0){
        frequency = pulseNum/pulseTime;
    }
    return frequency;
}

double MP412Device::TimerData2Time(const int timerData)
{
    //定时器单位0.1us
    return timerData*0.1/1000.0/1000.0;//返回秒数
}

void MP412Device::SetDOStatus(const int pinNum, const int status)
{
    static int doStatus = 0;//开关量输出状态

    if(status == LowLevel){
        doStatus &= (1<<pinNum)^0xffff;//清零对应位
    }else if(status == HighLevel){
        doStatus |= 1<<pinNum;//置一对应位
    }

    MP412_DO(devHandle, doStatus);
}
