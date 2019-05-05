#include "pedalrobotusbprocessor.h"

#include "configuration.h"

#include "common/printf.h"
#include "settingwidget/settingwidgetpedalrobotgetspeed.h"

PedalRobotUSBProcessor::PedalRobotUSBProcessor(QObject *parent) : QObject(parent)
{
    // 初始化数据采集
    m_canAnalyst = new CANAnalyst();
#ifdef PEDAL_ROBOT_API_DISABLE_CAN_DEVICE
    PRINTF(LOG_INFO, "%s: CanAnalyst is not opened!\n", __func__);
#else
    m_canAnalyst->OpenCanDevice();
#endif
    myMP412 = new MP412Device();
}

PedalRobotUSBProcessor::~PedalRobotUSBProcessor()
{
    delete myMP412;
    delete m_canAnalyst;
}

void PedalRobotUSBProcessor::UpdateCANData()
{
#ifdef FAKE_CAN
    double adjustParam = 100.0 - 100.0;

    if (RobotThetaMatrix::Instance()->GetTheta(1) < 12.0)
    {
        m_accOpenValue = 0.0;
    }
    else if (RobotThetaMatrix::Instance()->GetTheta(1) < 90.0)
    {
        m_accOpenValue = 100.0 * (std::pow((RobotThetaMatrix::Instance()->GetTheta(1) - adjustParam), 2) - std::pow((10.0 - adjustParam), 2)) /
                                                     (std::pow((90.0 - adjustParam), 2) - std::pow((10.0 - adjustParam), 2));
    }
    else
    {
        m_accOpenValue = 100.0;
    }

    m_brakeOpenValue = RobotThetaMatrix::Instance()->GetTheta(0) - 20.0;
    if (m_brakeOpenValue<=0.0) m_brakeOpenValue=0.0;

    m_usbCanCarSpeed += (m_accOpenValue /100.0 - m_brakeOpenValue / 50.0 - 0.01 - (m_usbCanCarSpeed * m_usbCanCarSpeed * 2.4406 / 100000 - m_usbCanCarSpeed * 1.2166 / 10000 + 0.0072));
    if (m_usbCanCarSpeed<=0.0) m_usbCanCarSpeed=0.0;
    if (m_usbCanCarSpeed>=145.0) m_usbCanCarSpeed=145.0;
#else
    int num = m_canAnalyst->CheckCanMessage();
    if(num <= 0){//no new message
        return;
    }

    m_brakeOpenValue = m_canAnalyst->GetBrakeOpenValue();
    m_accOpenValue = m_canAnalyst->GetAccOpenValue();
    m_usbCanCarSpeed = m_canAnalyst->GetCarSpeed();
#endif

    m_powerMode = m_canAnalyst->GetPowerMode();
}

void PedalRobotUSBProcessor::UpdateMP412Data()
{
    myMP412->UpdateData();

    double value=0.0, speed=0.0;
    switch (Configuration::Instance()->getSpeedMethod){
    case SettingWidgetPedalRobotGetSpeed::Voltage2Speed:
        value = myMP412->GetADVoltage();
        speed = value * Configuration::Instance()->ADCalibrateK;
        break;
    case SettingWidgetPedalRobotGetSpeed::PulseFrequency2Speed:
#ifdef FAKE_CAN
        speed = m_usbCanCarSpeed;
#else
        value = myMP412->GetPulseFrequency();
        speed = value * Configuration::Instance()->pulseCalibrateSpeed / Configuration::Instance()->pulseCalibrateFrequency;
#endif
        break;
    case SettingWidgetPedalRobotGetSpeed::CANSpeed:
        break;
    default:
        PRINTF(LOG_ERR, "%s: undefined getSpeedMethod\n", __func__);
        break;
    }

    m_usbMP412CarSpeed = std::max(speed, 0.0);
}

void PedalRobotUSBProcessor::UpdateDeviceControl()
{
    UpdateCANData(); // CAN上数据
    UpdateMP412Data(); // MP412上数据
}

void PedalRobotUSBProcessor::UpdateDeviceReady()
{
    //准备更新数据
#ifdef ENABLE_MP412_DEVICE
    //MP412测量脉冲频率 需要启动定时器后等待一段时间
    //这段时间内 可以进行其他的更新操作
    //故 在PedalRobotUI::PedalTimerDone()中先调用该函数，使得MP412的脉冲计数器开始工作
    //其他更新操作完成后 再去更新MP412上的数据(读取脉冲计数器和定时器的数据)

    bool isLowSpeed = m_usbMP412CarSpeed < Configuration::Instance()->pulseCounterSwitchSpeed;
    myMP412->StartPulseCounter(isLowSpeed ? MP412Device::ForLowFreq : MP412Device::ForHighFreq);
#endif
}

double PedalRobotUSBProcessor::GetBrakeOV()
{
    return m_brakeOpenValue;
}

double PedalRobotUSBProcessor::GetAcceleratorOV()
{
    return m_accOpenValue;
}

double PedalRobotUSBProcessor::GetCanCarSpeed()
{
    return m_usbCanCarSpeed;
}

int PedalRobotUSBProcessor::GetPowerMode()
{
    return m_powerMode;
}

double PedalRobotUSBProcessor::GetMP412CarSpeed()
{
    return m_usbMP412CarSpeed;
}

double PedalRobotUSBProcessor::GetAdVoltage()
{
    return myMP412->GetADVoltage();
}

double PedalRobotUSBProcessor::GetPulseFrequency()
{
    return myMP412->GetPulseFrequency();
}
