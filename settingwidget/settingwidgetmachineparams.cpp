#include "settingwidgetmachineparams.h"
#include "ui_settingwidgetmachineparams.h"

#include <assert.h>

SettingWidgetMachineParams::SettingWidgetMachineParams(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetMachineParams)
{
    ui->setupUi(this);

    InitLabel();
    InitLineEdit();
    InitDifferentRobot();
}

SettingWidgetMachineParams::~SettingWidgetMachineParams()
{
    delete ui;
}

void SettingWidgetMachineParams::LoadParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::machineParamNum; ++i){
        SetLineEditValue(lineEdit_machineParams[i], conf.machineParam[i]);
    }
}

bool SettingWidgetMachineParams::StoreParameters(Configuration &conf)
{
    for(int i=0; i<RobotParams::machineParamNum; ++i){
        conf.machineParam[i] = GetLineEditValue(lineEdit_machineParams[i]);
    }
    return true;
}

void SettingWidgetMachineParams::InitLabel()
{
    label_machineParams[0] = ui->label_11;
    label_machineParams[1] = ui->label_12;
    label_machineParams[2] = ui->label_13;
    label_machineParams[3] = ui->label_14;
    label_machineParams[4] = ui->label_15;
    label_machineParams[5] = ui->label_16;
    label_machineParams[6] = ui->label_17;

    label_machineParams[7] = ui->label_21;
    label_machineParams[8] = ui->label_22;
    label_machineParams[9] = ui->label_23;
    label_machineParams[10] = ui->label_24;
    label_machineParams[11] = ui->label_25;
    label_machineParams[12] = ui->label_26;
    label_machineParams[13] = ui->label_27;

    label_machineParams[14] = ui->label_31;
    label_machineParams[15] = ui->label_32;
    label_machineParams[16] = ui->label_33;
    label_machineParams[17] = ui->label_34;
    label_machineParams[18] = ui->label_35;
    label_machineParams[19] = ui->label_36;
    label_machineParams[20] = ui->label_37;
}

void SettingWidgetMachineParams::InitLineEdit()
{
    lineEdit_machineParams[0] = ui->lineEdit_11;
    lineEdit_machineParams[1] = ui->lineEdit_12;
    lineEdit_machineParams[2] = ui->lineEdit_13;
    lineEdit_machineParams[3] = ui->lineEdit_14;
    lineEdit_machineParams[4] = ui->lineEdit_15;
    lineEdit_machineParams[5] = ui->lineEdit_16;
    lineEdit_machineParams[6] = ui->lineEdit_17;

    lineEdit_machineParams[7] = ui->lineEdit_21;
    lineEdit_machineParams[8] = ui->lineEdit_22;
    lineEdit_machineParams[9] = ui->lineEdit_23;
    lineEdit_machineParams[10] = ui->lineEdit_24;
    lineEdit_machineParams[11] = ui->lineEdit_25;
    lineEdit_machineParams[12] = ui->lineEdit_26;
    lineEdit_machineParams[13] = ui->lineEdit_27;

    lineEdit_machineParams[14] = ui->lineEdit_31;
    lineEdit_machineParams[15] = ui->lineEdit_32;
    lineEdit_machineParams[16] = ui->lineEdit_33;
    lineEdit_machineParams[17] = ui->lineEdit_34;
    lineEdit_machineParams[18] = ui->lineEdit_35;
    lineEdit_machineParams[19] = ui->lineEdit_36;
    lineEdit_machineParams[20] = ui->lineEdit_37;
}

void SettingWidgetMachineParams::HideMachineParamsWidget(int startIndex, int endIndex)
{
    for(int i=startIndex; i<RobotParams::machineParamNum && i<endIndex; ++i){
        label_machineParams[i]->hide();
        lineEdit_machineParams[i]->hide();
    }
}

void SettingWidgetMachineParams::InitDifferentRobot()
{
    InitDof4Medical();
    InitDof2Pedal();
    InitDof5ScaraPaint();
    InitDof5IgusFlexibleShaft();
    InitDof6XyzVesselPuncture();
}

void SettingWidgetMachineParams::InitDof4Medical()
{
#ifdef DOF_4_MEDICAL
    label_machineParams[0]->setText("L1/mm");
    label_machineParams[1]->setText("L3/mm");
    label_machineParams[2]->setText("L4/mm");
    HideMachineParamsWidget(3, RobotParams::machineParamNum);
#endif
}

void SettingWidgetMachineParams::InitDof2Pedal()
{
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
    /* 运动学角度长度的换算 */
    label_machineParams[0]->setText(QObject::tr("丝杆导程/mm"));//实际丝杆导程
    /* 加踏板且踏板没到触碰位置 踏板多加的一部分 */
    label_machineParams[1]->setText(QObject::tr("刹车下压增量/mm"));//目前为2
    label_machineParams[2]->setText(QObject::tr("油门下压增量/mm"));//目前为10
    /* 单轮监听模式的运动限制 */
    label_machineParams[3]->setText(QObject::tr("每轮运动极限/mm"));//丝杆导程10mm对应30mm 导程8mm对应24mm
    /* ConfirmPedalPosition的速度 */
    label_machineParams[4]->setText(QObject::tr("寻找踏板触点的速度"));//丝杆导程10mm对应0.2 导程8mm对应0.25mm

    //单轴运动的速度倍率
    label_machineParams[5]->setText(QObject::tr("单轴运动速度倍率"));//丝杆导程10mm对应1.0 导程8mm对应3.75
    //是否显示PedalRobotUI中的lineEdit_sysControlMethod和lineEdit_PIDParams
    label_machineParams[6]->setText(QObject::tr("显示操作DEBUG状态"));

    HideMachineParamsWidget(7, RobotParams::machineParamNum);
#endif
}

void SettingWidgetMachineParams::InitDof5ScaraPaint()
{
#ifdef DOF_5_SCARA_PAINT
    label_machineParams[0]->setText(tr("1#轴齿轮分度圆直径/mm"));
    label_machineParams[1]->setText(tr("大臂长L2/mm"));
    label_machineParams[2]->setText(tr("小臂长L3/mm"));
    label_machineParams[3]->setText(tr("末端-4#旋转轴长L4/mm"));
    label_machineParams[4]->setText(tr("末端-4#旋转轴偏移L5/mm"));

    HideMachineParamsWidget(5, RobotParams::machineParamNum);
#endif
}

void SettingWidgetMachineParams::InitDof5IgusFlexibleShaft()
{
#ifdef DOF_5_IGUS_FLEXIBLE_SHAFT
    label_machineParams[0]->setText(tr("底座高L1/mm"));
    label_machineParams[1]->setText(tr("大臂长L2/mm"));
    label_machineParams[2]->setText(tr("小臂长L3/mm"));
    label_machineParams[3]->setText(tr("末端-4#旋转轴长L4/mm"));
    label_machineParams[4]->setText(tr("末端-4#旋转轴偏移L5/mm"));

    HideMachineParamsWidget(5, RobotParams::machineParamNum);
#endif
}

void SettingWidgetMachineParams::InitDof6XyzVesselPuncture()
{
#ifdef DOF_6_XYZ_VESSEL_PUNCTURE
    label_machineParams[0]->setText(tr("1#每转运动距离 mm/r"));
    label_machineParams[1]->setText(tr("2#每转运动距离 mm/r"));
    label_machineParams[2]->setText(tr("3#每转运动距离 mm/r"));

    label_machineParams[3]->setText(tr("4-6#轴的径向偏移D1 mm"));
    label_machineParams[4]->setText(tr("6#轴长D2 mm"));
    label_machineParams[5]->setText(tr("6#-末端的径向偏移D3 mm"));
    label_machineParams[6]->setText(tr("末端长度D4 mm"));

    HideMachineParamsWidget(7, RobotParams::machineParamNum);
#endif
}
