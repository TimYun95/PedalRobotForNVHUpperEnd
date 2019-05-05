/*机器人 封装API*/
/*有时关闭程序后 跳出APPCRASH的故障 选择兼容模式'Windows XP'运行即可 原因不明*/
#ifndef ROBOTAPI_H
#define ROBOTAPI_H

#include <QWidget>
#include <QLabel>
#include <QTimer>

#include "UnifiedRobotApiLibrary/apidllexportdefine.h"

class URI;

class UNIFIED_ROBOT_DLL_EXPORT RobotAPI : public QWidget
{
    Q_OBJECT
public:
    explicit RobotAPI(QWidget *parent = 0);
    ~RobotAPI();

public://API
    //一次设定后,机器运行过程中可以被再次设定(再次设定 将覆盖之前的设定)
    int SetAbsoluteAngle(int axisNum, double angle, double speed);//单轴绝对运动

    double GetCurrentAngle(int axisNum);//获得每个轴的关节量

    void GoHomeForAllAxis();//回原(机器刚开机后,需要运行一次)

    void SetForceControl(bool flag);//是否启用力控制 注意!力控时,过载检测ForceAlarm失效

    void StopMoving();//停止所有轴的运动

    int GetStatus();//获得机器人状态(请对照EXCEL表格)

    int EnterFastControl(double time=45);//time=大致需要的时间
    int SetAngleInFastControl(int axisNum, double absAngle);

signals:
    void FastControlOKSignal();

#ifdef ENABLE_UNIFIED_ROBOT_EXPORT_API_DLL
private slots:
    void ReceiveAxis4AngleSlot();

private:
    URI* pUri;
    QTimer* timerAPI;
    QLabel* statusLabel;//左下角状态
#endif
};

#endif // ROBOTAPI_H
