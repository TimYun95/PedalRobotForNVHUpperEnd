#include "cananalyst.h"

#include <windef.h>
#include <stdio.h>
#include <iostream>
#include <bitset>

#include "common/printf.h"

static constexpr int nDeviceType=4;//CANalyst-II对应型号为4
static constexpr int nDeviceInd=0;//第一个USB-CAN适配器
static constexpr int CANPort[2]={0,1};
static constexpr int CANSendPort=CANPort[0];
static constexpr int CANReceivePort=CANPort[1];

static constexpr UINT BrakeID=0x0F1;
static constexpr UINT AcceleratorID=0x0C9;
static constexpr UINT CarSpeedID=0x3E9;
static constexpr UINT PowerModeID=0x1F1;
static constexpr UINT AcdID=0x7FE;

static constexpr bool EnablePrint=false;

CANAnalyst::CANAnalyst()
    : m_status(Closed),
      m_brakeOpenValue(1.0),
      m_accOpenValue(1.0),
      m_flagRecvCarSpeed(false),
      m_carSpeed(1.0),
      m_powerMode(1),
      m_mTApp(0),
      m_transReqGr(1),
      m_cmndMd(3),
      m_vehStSpd(1.0),
      m_acclStPos(1.0),
      m_acclDlyTmr(1.0),
      m_brkPdlStPos(1.0),
      m_brkPdlDlyTmr(1.0)
{

}

CANAnalyst::~CANAnalyst()
{
    if(m_status == Running){
        CloseCanDevice();
    }
}

int CANAnalyst::OpenCanDevice()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    // open
    DWORD dwRel = VCI_OpenDevice(nDeviceType, nDeviceInd, 0);
    if(dwRel != 1){
        PRINTF(LOG_ERR, "%s: cannot open the USB-CAN device\n", __func__);
        return -1;
    }

    // init
    VCI_INIT_CONFIG vic;
    vic.AccCode=0x80000008;
    vic.AccMask=0xFFFFFFFF;
    vic.Filter=1;
    vic.Timing0=0x00;//500KbpS
    vic.Timing1=0x1C;
    vic.Mode=0;
    dwRel = VCI_InitCAN(nDeviceType, nDeviceInd, CANPort[0], &vic);
    if(dwRel != 1){
        PRINTF(LOG_ERR, "%s: fail to init USB-CAN device(port 0)\n", __func__);
        return -1;
    }
    dwRel = VCI_InitCAN(nDeviceType, nDeviceInd, CANPort[1], &vic);
    if(dwRel != 1){
        PRINTF(LOG_ERR, "%s: fail to init USB-CAN device(port 1)\n", __func__);
        return -1;
    }

    // start
    dwRel = VCI_StartCAN(nDeviceType, nDeviceInd, CANPort[0]);
    if(dwRel != 1){
        PRINTF(LOG_ERR, "%s: fail to start USB-CAN device(port 0)\n", __func__);
        return -1;
    }
    dwRel = VCI_StartCAN(nDeviceType, nDeviceInd, CANPort[1]);
    if(dwRel != 1){
        PRINTF(LOG_ERR, "%s: fail to start USB-CAN device(port 1)\n", __func__);
        return -1;
    }

    m_status = Running;
    return 0;
}

int CANAnalyst::CloseCanDevice()
{
    DWORD dwRel = VCI_CloseDevice(nDeviceType, nDeviceInd);
    if(dwRel != 1){
        PRINTF(LOG_ERR, "%s: cannot close the USB-CAN device\n", __func__);
        return -1;
    }

    m_status = Closed;
    return 0;
}

int CANAnalyst::CheckCanMessage()
{
    if(m_status != Running){
        return -1;
    }

    //1. 全部读取
    DWORD dwRel = VCI_Receive(nDeviceType, nDeviceInd, CANReceivePort, vco_read, sizeof(vco_read)/sizeof(vco_read[0]), 0);
    if(dwRel == (DWORD)-1){
        //PRINTF(LOG_ERR, "%s: error! fail to receive message\n", __func__);
        return 0;
    }else if(dwRel == 0){
        //PRINTF(LOG_ERR, "%s: no new message\n", __func__);
        return 0;
    }
    //PRINTF(LOG_ERR, "%s: receive %d frames messages\n", __func__, (int)dwRel);
    PrintCanMseeage(vco_read,dwRel);

    //2. 读取解析所需的数据
    ReadRequiredData(dwRel);

    return dwRel;
}

void CANAnalyst::ReadRequiredData(int receiveNum)
{
    m_flagRecvCarSpeed = false;//清零

    //数据格式转换
    std::bitset<5> foundset(0);
    for(int i=0; i<receiveNum; ++i) {
        if(vco_read[i].ID == BrakeID) {
            BYTE buff = vco_read[i].Data[1];//Start Byte=1, Len=8bits
            unsigned int num=buff;
            m_brakeOpenValue=num*0.392157;

            foundset.set(0);
        }else if(vco_read[i].ID == AcceleratorID) {
            BYTE buff = vco_read[i].Data[4];//Start Byte=4, Len=8bits
            unsigned int num=buff;
            m_accOpenValue=num*0.392157;

            foundset.set(1);
        }else if(vco_read[i].ID == CarSpeedID) {
            BYTE buff[2];
            buff[0] = vco_read[i].Data[0];//Start Byte=0, Len=15bits
            buff[1] = vco_read[i].Data[1];
            buff[0] &= 0x7F;//高位D7=0
            unsigned int num=0;
            num |= ((unsigned int)buff[0])<<8;
            num |= buff[1];
            m_carSpeed=num*0.015625;

            foundset.set(2);
            m_flagRecvCarSpeed = true;//本轮CAN数据帧中有车速信号
        }else if(vco_read[i].ID == PowerModeID) {
            BYTE buff = vco_read[i].Data[0];//Start Byte=0, Len=2bits
            buff &= 0x03;
            unsigned int num=buff;
            m_powerMode=num;

            foundset.set(3);
        }
        else if(vco_read[i].ID == AcdID) {
            //是否手动挡
            BYTE buff1 = vco_read[i].Data[0];//Start Byte=0, Start Bit=7,Len=1bits
            buff1 &= 0x80;
            unsigned int num1=buff1;
            m_mTApp=num1;

            //手动档位
            BYTE buff2 = vco_read[i].Data[0];//Start Byte=0, Start Bit=6,Len=4bits
            buff2 &= 0x78;
            unsigned int num2=buff2;
            m_transReqGr=num2;

            //控制模式
            BYTE buff3 = vco_read[i].Data[0];//Start Byte=0, Start Bit=2,Len=2bits
            buff3 &= 0x06;
            unsigned int num3=buff3;
            m_cmndMd=num3;

            //车速
            BYTE buff4[2];//Start Byte=1, Start Bit=7,Len=15bits
            buff4[0] = vco_read[i].Data[1];
            buff4[1] = vco_read[i].Data[2];
            buff4[1] &= 0xFE;//低位D16=0
            unsigned int num4=0;
            num4 |= ((unsigned int)buff4[0])<<8;//左移8位
            num4 |= buff4[1];
            m_vehStSpd=num4*0.015625;

            //油门目标开度
            BYTE buff5 = vco_read[i].Data[3];//Start Byte=3, Start Bit=7,Len=8bits
            unsigned int num5=buff5;
            m_acclStPos=num5*0.392157;

            //油门延迟时间
            BYTE buff6 = vco_read[i].Data[4];//Start Byte=4, Start Bit=7,Len=8bits
            unsigned int num6=buff6;
            m_acclDlyTmr=num6*0.1;

            //刹车目标开度
            BYTE buff7 = vco_read[i].Data[5];//Start Byte=5, Start Bit=7,Len=8bits
            unsigned int num7=buff7;
            m_brkPdlStPos=num7*0.392157;

            //刹车延迟时间
            BYTE buff8 = vco_read[i].Data[6];//Start Byte=6, Start Bit=7,Len=8bits
            unsigned int num8=buff8;
            m_brkPdlDlyTmr=num8*0.1;

            foundset.set(4);
        }

        if(foundset.all()){//数据全部找到 退出for循环 加快读取速度
            break;
        }
    }
}

double CANAnalyst::GetBrakeOpenValue()
{
    return m_brakeOpenValue;
}

double CANAnalyst::GetAccOpenValue()
{
    return m_accOpenValue;
}

bool CANAnalyst::IfRecvCarSpeed()
{
    return m_flagRecvCarSpeed;
}

double CANAnalyst::GetCarSpeed()
{
    return m_carSpeed;
}

int CANAnalyst::GetPowerMode()
{
    return m_powerMode;
}

int CANAnalyst::GetMTApp()
{
    return m_mTApp;
}

int CANAnalyst::GetTransReqGr()
{
    return m_transReqGr;
}

int CANAnalyst::GetCmndMd()
{
    return m_cmndMd;
}

double CANAnalyst::GetVehStSpd()
{
    return m_vehStSpd;
}

double CANAnalyst::GetAcclStPos()
{
    return m_acclStPos;
}

double CANAnalyst::GetAcclDlyTmr()
{
    return m_acclDlyTmr;
}

double CANAnalyst::GetBrkPdlStPos()
{
    return m_brkPdlStPos;
}

double CANAnalyst::GetBrkPdlDlyTmr()
{
    return m_brkPdlDlyTmr;
}


void CANAnalyst::SetCanAnalystData(const std::vector<double> &canData)
{
    if(canData.size() != CANAnalyst::CanDataSize)
    {
        PRINTF(LOG_ERR, "%s: error can data size(%d)\n", __func__, canData.size());
        return;
    }

#ifdef DOF_3_PEDAL_API
    const double &brakeOpenValue = canData[0];
    const double &accOpenValue = canData[1];
    const double &carSpeed = canData[2];
    int powerMode = canData[3];
    int mTApp = canData[4];
    int transReqGr = canData[5];
    int cmndMd = canData[6];
    const double &vehStSpd = canData[7];
    const double &acclStPos = canData[8];
    const double &acclDlyTmr = canData[9];
    const double &brkPdlStPos = canData[10];
    const double &brkPdlDlyTmr = canData[11];

    m_brakeOpenValue = brakeOpenValue;
    m_accOpenValue = accOpenValue;
    m_carSpeed = carSpeed;
    m_powerMode = powerMode;

    m_mTApp = mTApp;
    m_transReqGr = transReqGr;
    m_cmndMd = cmndMd;
    m_vehStSpd = vehStSpd;
    m_acclStPos = acclStPos;
    m_acclDlyTmr = acclDlyTmr;
    m_brkPdlStPos = brkPdlStPos;
    m_brkPdlDlyTmr = brkPdlDlyTmr;
#else
    PRINTF(LOG_CRIT, "%s: this function should only be used in DOF_3_PEDAL_API!\n", __func__);
    Q_ASSERT(false);
#endif
}

std::vector<double> CANAnalyst::GetCanAnalystData()
{
    std::vector<double> canData(CanDataSize, 0.0);
    canData[0] = m_brakeOpenValue;
    canData[1] = m_accOpenValue;
    canData[2] = m_carSpeed;
    canData[3] = m_powerMode;

    canData[4] = m_mTApp;
    canData[5] = m_transReqGr;
    canData[6] = m_cmndMd;
    canData[7] = m_vehStSpd;
    canData[8] = m_acclStPos;
    canData[9] = m_acclDlyTmr;
    canData[10] = m_brkPdlStPos;
    canData[11] = m_brkPdlDlyTmr;

    return canData;
}

void CANAnalyst::PrintCanMseeage(const VCI_CAN_OBJ *obj, const DWORD &size)
{
    if(!EnablePrint){
        return;
    }

    PRINTF(LOG_DEBUG, "%s starts\n",__func__);
    for(DWORD i=0; i<size; i++){
        PRINTF(LOG_DEBUG, ".....data[%d].....\n", (int)i);
        PRINTF(LOG_ERR, " ID=%d\n", obj[i].ID);

        int len=obj[i].DataLen;
        PRINTF(LOG_DEBUG, " DataLen=%d\n", len);
        for(int j=0; j<len; j++){
            int data=obj[i].Data[j];
            PRINTF(LOG_DEBUG, " 0x%02x", data);
        }
        PRINTF(LOG_DEBUG, "\n");
    }
}

