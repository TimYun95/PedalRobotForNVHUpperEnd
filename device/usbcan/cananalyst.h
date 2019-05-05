#ifndef CANANALYST_H
#define CANANALYST_H

#include <vector>
#include "ControlCAN.h"

//泛亚CAN口引脚定义 6=H 14=L 5=SigGnd 4=CarGnd

class CANAnalyst
{
public:
    explicit CANAnalyst();
    virtual ~CANAnalyst();

    enum DeviceRunningStatus{
        Closed,
        Running
    };

    int OpenCanDevice();//-1=error 0=ok
    int CloseCanDevice();

    int CheckCanMessage();//检查接收到的消息 返回接收到的消息数量

    double GetBrakeOpenValue();//获取刹车开度
    double GetAccOpenValue();//获取油门开度
    bool IfRecvCarSpeed();//本轮数据中是否有车速数据
    double GetCarSpeed();//获取车辆当前速度
    int GetPowerMode();//获取车辆动力模式

    int GetMTApp();//是否手动挡
    int GetTransReqGr();//手动档位
    int GetCmndMd();//控制模式
    double GetVehStSpd();//目标速度
    double GetAcclStPos();//目标油门开度
    double GetAcclDlyTmr();//油门延迟时间
    double GetBrkPdlStPos();//目标刹车开度
    double GetBrkPdlDlyTmr();//刹车延迟时间

    //特殊情况 需要设定CANAnalyst的数据(用于DOF_3_PEDAL_API)
    enum eCanDataSize{
        CanDataSize = 12
    };
    //使用vector请注意检查index顺序
    void SetCanAnalystData(const std::vector<double> &canData);
    std::vector<double> GetCanAnalystData();

protected:
    void ReadRequiredData(int receiveNum);//读取所需的数据
    void PrintCanMseeage(const VCI_CAN_OBJ *obj, const DWORD& size);//输出Debug消息内容

private:
    VCI_CAN_OBJ vco_read[2500];//2500 is big enough(the size of buff in CAN device is only 2000)

    int m_status;

    double m_brakeOpenValue;
    double m_accOpenValue;
    bool m_flagRecvCarSpeed;
    double m_carSpeed;
    int m_powerMode;

    int m_mTApp;
    int m_transReqGr;
    int m_cmndMd;
    double m_vehStSpd;
    double m_acclStPos;
    double m_acclDlyTmr;
    double m_brkPdlStPos;
    double m_brkPdlDlyTmr;
};

#endif // CANANALYST_H
