#include "uri.h"
#include "ui_uri.h"

#include <math.h>
#include <stdlib.h>

#include <fstream>
#include <algorithm>
#include <tr1/functional>

#include <QStringList>
#include <QFileDialog>
#include <QInputDialog>

#include "common/printf.h"
#include "common/globalvariables.h"
#include "common/sectioninfo.h"
#include "communication/msgtypes.h"

#include "fileoperation/normalfile.h"
#include "util/timeutil.h"
#include "assistantfunc/fileassistantfunc.h"
#include "settingwidget/settingwidgetinclude.h"
#include "mywidget/mymessagebox.h"
#include "mywidget/mywidgethelper.h"
#include "miscconfigurationparam.h"

namespace ActionStatus {
    enum tagActionStatus {
        PAUSED,
        STOPPED,
        ERROR_LIMIT,
        FINISHED,
        RUNNING
    };
}

//URI=UnifiedRobotInterface
URI::URI(QWidget *parent, bool enableLogicUpdateTimer, bool enableWidgetUpdateTimer)
    : QWidget(parent),
      ui(new Ui::URI)
    #ifdef NVH_USED
    , rootPwd("Root1005!")
    #endif
{
    ui->setupUi(this);

    UnifiedRobot::CreateInstance();
    UnifiedRobot::Instance()->Init();

    m_unifiedRobot = UnifiedRobot::Instance();
    mTeach = new Teach();
    conf = Configuration::Instance();
    rbtThetaMatrix = RobotThetaMatrix::Instance();
    m_settingLevel = SettingBase::UnLoggedIn;
    ui->widget_2DGraph->layout()->addWidget(&m_paintGraph);

    //默认作为界面 需要弹窗提示
    MiscConfigurationParam::Instance()->SetConfigParam(MiscConfigurationParam::eEnableShowBoxWhenFinishGoHome, true);
    MiscConfigurationParam::Instance()->SetConfigParam(MiscConfigurationParam::eEnableShowBoxWhenReceiveMessageInform, true);

    StatusString::Instance()->SetStatusIndex(StatusString::Status_ConnectiongCommunication);

    InitWidgetDisplay();
    InitSettingsWidgets();
    CustomInitWithRobots();

    InitSignalSlot();
    RegisterUpdateActionCallback();

    if(enableLogicUpdateTimer){
        m_logicUpdateTimer.StartTimer(RobotParams::logicUpdateTimer_IntervalMs, RobotParams::logicUpdateTimer_IntervalMultiplier);
    }
    if(enableWidgetUpdateTimer){
        m_widgetUpdateTimer.StartTimer(RobotParams::widgetUpdateTimer_IntervalMs, RobotParams::widgetUpdateTimer_IntervalMultiplier);
    }

    std::cout<< RobotParams::robotType <<std::endl;
}

URI::~URI()
{
    delete mTeach;
    UnifiedRobot::Instance()->Uninit();
    UnifiedRobot::DestroyInstance();
    delete ui;

    std::cout<<"exit URI"<<std::endl;
}

void URI::UpdateLogic()
{
    qint64 currentTimestampMs = TimeUtil::CurrentTimestampMs();
    m_unifiedRobot->Update(currentTimestampMs);

    UpdateLogic_MedicalUltrasoundRobot();

    fflush(stdout);//输出PRINTF的内容 不要缓存
}

void URI::UpdateWidgets()
{
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
#ifdef NVH_USED
    return;
#else
    if(conf->pedalRobotUsage == SettingWidgetPedalRobotGetSpeed::ManualEncoderControl){
        //DEBUG下需要刷新
    }else{
        return;//不需要URI的UI刷新
    }
#endif
#endif

    UpdateWidgetByThetaMatrix();
    UpdateWidgetByActionMsg();
    UpdateWidgetByCmtConnection();
}

void URI::InitSettingsWidgets()
{
    QListWidgetItem* runItem=new QListWidgetItem(QObject::tr("运行"));
    settingMap[runItem]=new SettingWidgetRun();
    QListWidgetItem* machineItem=new QListWidgetItem(QObject::tr("机构"));
    settingMap[machineItem]=new SettingWidgetMachine();
    QListWidgetItem* encoderItem=new QListWidgetItem(QObject::tr("码盘"));
    settingMap[encoderItem]=new SettingWidgetEncoder();
    QListWidgetItem* limitItem=new QListWidgetItem(QObject::tr("极限"));
    settingMap[limitItem]=new SettingWidgetLimit();
    QListWidgetItem* motorItem=new QListWidgetItem(QObject::tr("电机"));
    settingMap[motorItem]=new SettingWidgetMotor();
    QListWidgetItem* pidItem=new QListWidgetItem(QObject::tr("PID"));
    settingMap[pidItem]=new SettingWidgetPID();
    QListWidgetItem* speedItem=new QListWidgetItem(QObject::tr("速度"));
    settingMap[speedItem]=new SettingWidgetSpeed();
    QListWidgetItem* interfaceItem=new QListWidgetItem(QObject::tr("接口"));
    settingMap[interfaceItem]=new SettingWidgetHardwareInterface();
    QListWidgetItem* directionItem=new QListWidgetItem(QObject::tr("方向"));
    settingMap[directionItem]=new SettingWidgetDirection();
    QListWidgetItem* errorLimitItem=new QListWidgetItem(QObject::tr("误差"));
    settingMap[errorLimitItem]=new SettingWidgetErrorLimit();
    QListWidgetItem* machineParamsItem=new QListWidgetItem(QObject::tr("参数"));
    settingMap[machineParamsItem]=new SettingWidgetMachineParams();
    QListWidgetItem* singleAxisItem=new QListWidgetItem(QObject::tr("单轴"));
    settingMap[singleAxisItem]=new SettingWidgetSingleAxis();
    QListWidgetItem* manualControlItem=new QListWidgetItem(QObject::tr("手动"));
    settingMap[manualControlItem]=new SettingWidgetManualControl();
    QListWidgetItem* maxMonitorDiffThetaItem=new QListWidgetItem(QObject::tr("监听"));
    settingMap[maxMonitorDiffThetaItem]=new SettingWidgetMaxMonitorDiffTheta();

#ifdef USE_ABS_ENCODER_WIDGET//伺服 读取绝对编码器
    QListWidgetItem* absValueItem=new QListWidgetItem(QObject::tr("伺服"));
    settingMap[absValueItem]=new SettingWidgetAbsValueAtOrigin();
#else//光电设定回原速度
    QListWidgetItem* goHomeSpeedItem=new QListWidgetItem(QObject::tr("回原"));
    settingMap[goHomeSpeedItem]=new SettingWidgetGoHomeSpeed();
#endif

#ifdef ENABLE_WEBSOCKET_COMMUNICATION
    QListWidgetItem* webSocketItem = new QListWidgetItem(QObject::tr("WSPP"));
    settingMap[webSocketItem] = new SettingWidgetWebSocket();
#endif

    InitSettingsWidgetWithRobots();

    //add
    for(std::map<QListWidgetItem*,SettingBase*>::iterator iter=settingMap.begin(); iter!=settingMap.end(); iter++){
        SettingBase* sb = iter->second;
        sb->LoadParameters(*conf);
        QWidget* w=dynamic_cast<QWidget*>(sb);
        if(w){
            ui->widget_settings->layout()->addWidget(w);
        }
        DisplaySettingWidget(sb,false);
    }

    ui->pushButton_changePassword->setEnabled(false);
    ui->pushButton_saveSettings->setEnabled(false);
}

void URI::InitSettingsWidgetWithRobots()
{
    //不同机器人特有的设置
#ifdef ENABLE_FORCE_CONTROL_WIDGET
    QListWidgetItem* forceControlItem=new QListWidgetItem(QObject::tr("力控"));
    SettingWidgetForceControl *swfc=new SettingWidgetForceControl();
    settingMap[forceControlItem]=swfc;
    QObject::connect(swfc, SIGNAL(ForceControlClickedSignal(bool)), &m_medicalUltrasoundRobot, SLOT(ForceControlClickedSlot(bool)));
    QObject::connect(&m_medicalUltrasoundRobot, SIGNAL(ForceValueSignal(QString)), swfc, SLOT(ForceValueSlot(QString)));
#endif

#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
#ifdef NVH_USED
    QListWidgetItem* pdGetSpeed=new QListWidgetItem(QObject::tr("测速"));
    settingMap[pdGetSpeed]= new SettingWidgetPedalRobotNVHGetSpeed();
    QListWidgetItem* pdDeathZone=new QListWidgetItem(QObject::tr("死区"));
    settingMap[pdDeathZone]=new SettingWidgetPedalRobotNVHDeathZone();
    QListWidgetItem* pdControlParamter=new QListWidgetItem(QObject::tr("控制"));
    settingMap[pdControlParamter]=new SettingWidgetPedalRobotNVHControlParameter();
#else
    QListWidgetItem* pdGetSpeed=new QListWidgetItem(QObject::tr("测速"));
    settingMap[pdGetSpeed]= new SettingWidgetPedalRobotGetSpeed();
    QListWidgetItem* pdDeathZone=new QListWidgetItem(QObject::tr("死区"));
    settingMap[pdDeathZone]=new SettingWidgetPedalRobotDeathZone();

    if(conf->pedalRobotUsage == SettingWidgetPedalRobotGetSpeed::NedcControl){
        QListWidgetItem* study=new QListWidgetItem(QObject::tr("学习"));
        SettingWidgetPedalRobotStudy* studyWidget = new SettingWidgetPedalRobotStudy();
        settingMap[study]=studyWidget;
        connect(studyWidget, SIGNAL(UpdateNedcParamsSignal()), this, SLOT(on_pushButton_saveSettings_clicked()));
    }else{
        QListWidgetItem* studyWltc=new QListWidgetItem(QObject::tr("学习"));
        SettingWidgetPedalRobotStudyWltc* studyWltcWidget = new SettingWidgetPedalRobotStudyWltc();
        settingMap[studyWltc]=studyWltcWidget;
        connect(studyWltcWidget, SIGNAL(UpdateWltcParamsSignal()), this, SLOT(on_pushButton_saveSettings_clicked()));
    }
#endif
#endif
}

void URI::DisplaySettingWidget(SettingBase *settingBase, bool showFlag)
{
    QWidget* widget = dynamic_cast<QWidget*>(settingBase);
    if(widget == Q_NULLPTR){
        PRINTF(LOG_ERR, "%s: invalid setting base widget!\n", __func__);
    }

    if(showFlag) {
        widget->show();
    }else{
        widget->hide();
    }
}

void URI::ShowSettingWidgets(int settingLevel)
{
    for(std::map<QListWidgetItem*,SettingBase*>::iterator iter=settingMap.begin();iter!=settingMap.end();iter++){
        SettingBase* sb=iter->second;
        QListWidgetItem* lwi=iter->first;

        if(settingLevel >= sb->level){
            ui->listWidget_settings->addItem(lwi);
        }
    }

    ui->listWidget_settings->sortItems();
    ui->listWidget_settings->setCurrentRow(0);
}

void URI::HideSettingWidgets()
{
    while(ui->listWidget_settings->count()){
        ui->listWidget_settings->takeItem(0);
    }

    for(std::map<QListWidgetItem*,SettingBase*>::iterator iter=settingMap.begin();iter!=settingMap.end();iter++){
        SettingBase* sb=iter->second;
        DisplaySettingWidget(sb,false);
    }
}

void URI::DisplayWidget_AtSuspendPoint()
{
    ui->pushButton_startToWork->setEnabled(true);
    ui->pushButton_startToWork->setText(QObject::tr("继续工作"));
}

void URI::DisplayWidget_FinishGoHome(int goHomeResult)
{
    bool enableShowInformBox = MiscConfigurationParam::Instance()->GetConfigParam(MiscConfigurationParam::eEnableShowBoxWhenFinishGoHome);

    switch (goHomeResult) {
    case MsgStructure::GoHomeResult::MovingToTargetPointSuccessfully:
        PRINTF(LOG_DEBUG, "%s: MovingToTargetPointSuccessfully\n",__func__);
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
        if(enableShowInformBox){
#ifdef NVH_USED
          emit Signal_WidgetShowInformMessageBox("回原后运动到指定点完成!可以开始操作!");
#else
            MyMessageBox::ShowInformMessageBox("回原后运动到指定点完成!可以开始操作!");
#endif
        }
#endif
        //fall through
    case MsgStructure::GoHomeResult::Success:
        PRINTF(LOG_DEBUG, "%s: Success\n",__func__);

#ifndef NVH_USED
        EnableGoHomeWidgets(true);
#endif

        emit Signal_RobotGoHomeResult(true);
        break;
    case MsgStructure::GoHomeResult::Fail:
        PRINTF(LOG_DEBUG, "%s: Fail\n",__func__);
        emit Signal_RobotGoHomeResult(false);
        if(enableShowInformBox){
#ifdef NVH_USED
          emit Signal_WidgetShowInformMessageBox("回原失败!\n检查电路或再试一次!");
#else
            MyMessageBox::ShowInformMessageBox("回原失败!\n检查电路或再试一次!");
#endif
        }
        break;
    case MsgStructure::GoHomeResult::RequestTeachFile:
        break;
    case MsgStructure::GoHomeResult::MovingToTargetPoint:
        PRINTF(LOG_DEBUG, "%s: MovingToTargetPoint...\n", __func__);
        break;
    case MsgStructure::GoHomeResult::MovingToTargetPointUnsuccessfully:
        PRINTF(LOG_DEBUG, "%s: MovingToTargetPointUnSuccessfully\n",__func__);
        if(enableShowInformBox){
#ifdef NVH_USED
          emit Signal_WidgetShowInformMessageBox("回原后运动到指定点失败!\n检查电路或再试一次!");
#else
            MyMessageBox::ShowInformMessageBox("回原后运动到指定点失败!\n检查电路或再试一次!");
#endif
        }
        break;
    default:
        PRINTF(LOG_INFO, "%s: undefined result(%d)\n", __func__, goHomeResult);
        break;
    }
}

void URI::EnableGoHomeWidgets(bool enableFlag)
{
    ui->tab2->setEnabled(enableFlag);
    ui->tab3->setEnabled(enableFlag);
    ui->tab12->setEnabled(enableFlag);
    ui->tab13->setEnabled(enableFlag);
}

void URI::ResetSingleAxisWidgets()
{
    ui->horizontalSlider_axis1->setValue(0);
    ui->horizontalSlider_axis2->setValue(0);
    ui->horizontalSlider_axis3->setValue(0);
    ui->horizontalSlider_axis4->setValue(0);
    ui->horizontalSlider_axis5->setValue(0);
    ui->horizontalSlider_axis6->setValue(0);
}

void URI::HideSingleAxisWidget(int startAxis)
{
    switch (startAxis) {
    case 1://单轴axis1
        ui->pushButton_minusAxis1->hide();
        ui->pushButton_plusAxis1->hide();
        ui->horizontalSlider_axis1->hide();
        ui->pushButton_stopAxis1->hide();
        ui->label_singleAxis1->hide();
        ui->label_theta1->hide();
        ui->lineEdit_theta1->hide();;
        ui->lineEdit_inSignal1->hide();
        break;
    case 2://单轴axis1
        ui->pushButton_minusAxis2->hide();
        ui->pushButton_plusAxis2->hide();
        ui->horizontalSlider_axis2->hide();
        ui->pushButton_stopAxis2->hide();
        ui->label_singleAxis2->hide();
        ui->label_theta2->hide();
        ui->lineEdit_theta2->hide();;
        ui->lineEdit_inSignal2->hide();
        break;
    case 3://单轴axis3
        ui->pushButton_minusAxis3->hide();
        ui->pushButton_plusAxis3->hide();
        ui->horizontalSlider_axis3->hide();
        ui->pushButton_stopAxis3->hide();
        ui->label_singleAxis3->hide();
        ui->label_theta3->hide();
        ui->lineEdit_theta3->hide();;
        ui->lineEdit_inSignal3->hide();
        break;
    case 4://单轴axis4
        ui->pushButton_minusAxis4->hide();
        ui->pushButton_plusAxis4->hide();
        ui->horizontalSlider_axis4->hide();
        ui->pushButton_stopAxis4->hide();
        ui->label_singleAxis4->hide();
        ui->label_theta4->hide();
        ui->lineEdit_theta4->hide();;
        ui->lineEdit_inSignal4->hide();
        break;
    case 5://单轴axis5
        ui->pushButton_minusAxis5->hide();
        ui->pushButton_plusAxis5->hide();
        ui->horizontalSlider_axis5->hide();
        ui->pushButton_stopAxis5->hide();
        ui->label_singleAxis5->hide();
        ui->label_theta5->hide();
        ui->lineEdit_theta5->hide();;
        ui->lineEdit_inSignal5->hide();
        break;
    case 6://单轴axis6
        ui->pushButton_minusAxis6->hide();
        ui->pushButton_plusAxis6->hide();
        ui->horizontalSlider_axis6->hide();
        ui->pushButton_stopAxis6->hide();
        ui->label_singleAxis6->hide();
        ui->label_theta6->hide();
        ui->lineEdit_theta6->hide();;
        ui->lineEdit_inSignal6->hide();
        break;
    default:
        break;
    }
}

int URI::SingleAxisSlider2AxisIndex(QSlider *singleAxisSlider)
{
    if(singleAxisSlider == ui->horizontalSlider_axis1){
        return 0;
    }else if(singleAxisSlider == ui->horizontalSlider_axis2){
        return 1;
    }else if(singleAxisSlider == ui->horizontalSlider_axis3){
        return 2;
    }else if(singleAxisSlider == ui->horizontalSlider_axis4){
        return 3;
    }else if(singleAxisSlider == ui->horizontalSlider_axis5){
        return 4;
    }else if(singleAxisSlider == ui->horizontalSlider_axis6){
        return 5;
    }

    PRINTF(LOG_ERR, "%s: invalid single axis slider!\n", __func__);
    return 0;
}

double URI::SingleAxisSlider2Speed(QSlider *singleAxisSlider)
{
    int axisIndex = SingleAxisSlider2AxisIndex(singleAxisSlider);
    if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: axisIndex(%d) out of range.\n", __func__, axisIndex);
        return 0.0;
    }

    int speedLevel = singleAxisSlider->value();
    double singleAxisSpeed = speedLevel * conf->singleAxisSliderRatio[axisIndex];
    return singleAxisSpeed;
}

int URI::SingleAxisButton2AxisIndex(QPushButton *btn)
{
    if(btn == ui->pushButton_plusAxis1){
        return 0;
    }else if (btn == ui->pushButton_plusAxis2){
        return 1;
    }else if (btn == ui->pushButton_plusAxis3){
        return 2;
    }else if (btn == ui->pushButton_plusAxis4){
        return 3;
    }else if (btn == ui->pushButton_plusAxis5){
        return 4;
    }else if (btn == ui->pushButton_plusAxis6){
        return 5;
    }else if (btn == ui->pushButton_minusAxis1){
        return 0;
    }else if (btn == ui->pushButton_minusAxis2){
        return 1;
    }else if (btn == ui->pushButton_minusAxis3){
        return 2;
    }else if (btn == ui->pushButton_minusAxis4){
        return 3;
    }else if (btn == ui->pushButton_minusAxis5){
        return 4;
    }else if (btn == ui->pushButton_minusAxis6){
        return 5;
    }

    PRINTF(LOG_ERR, "%s: invalid button!\n", __func__);
    return 0;
}

int URI::SingleAxisButton2Direction(QPushButton *btn)
{
    if(btn == ui->pushButton_plusAxis1){//+
        return 1;
    }else if (btn == ui->pushButton_plusAxis2){
        return 1;
    }else if (btn == ui->pushButton_plusAxis3){
        return 1;
    }else if (btn == ui->pushButton_plusAxis4){
        return 1;
    }else if (btn == ui->pushButton_plusAxis5){
        return 1;
    }else if (btn == ui->pushButton_plusAxis6){
        return 1;
    }else if (btn == ui->pushButton_minusAxis1){//-
        return -1;
    }else if (btn == ui->pushButton_minusAxis2){
        return -1;
    }else if (btn == ui->pushButton_minusAxis3){
        return -1;
    }else if (btn == ui->pushButton_minusAxis4){
        return -1;
    }else if (btn == ui->pushButton_minusAxis5){
        return -1;
    }else if (btn == ui->pushButton_minusAxis6){
        return -1;
    }

    PRINTF(LOG_ERR, "%s: invalid button!\n", __func__);
    return  1;
}

manual_direction URI::ManualControlButton2Direction(QPushButton *btn)
{
    manual_direction direction = md_stop;
    if(btn == ui->pushButton_manualControlXPlus){//X
        direction=md_xplus;
    }else if(btn == ui->pushButton_manualControlXMinus){
        direction=md_xminus;
    }else if(btn == ui->pushButton_manualControlYPlus){//Y
        direction=md_yplus;
    }else if(btn == ui->pushButton_manualControlYMinus){
        direction=md_yminus;
    }else if(btn == ui->pushButton_manualControlZPlus){//Z
        direction=md_zplus;
    }else if(btn ==ui->pushButton_manualControlZMinus){
        direction=md_zminus;
    }else if(btn ==ui->pushButton_manualControlRXPlus){//RX
        direction=md_rxplus;
    }else if(btn == ui->pushButton_manualControlRXMinus){
        direction=md_rxminus;
    }else if(btn == ui->pushButton_manualControlRYPlus){//RY
        direction=md_ryplus;
    }else if(btn == ui->pushButton_manualControlRYMinus){
        direction=md_ryminus;
    }else if(btn == ui->pushButton_manualControlRZPlus){//RZ
        direction=md_rzplus;
    }else if(btn == ui->pushButton_manualControlRZMinus){
        direction=md_rzminus;
    }else{
        PRINTF(LOG_ERR, "%s: unvalid btn!\n", __func__);
        return md_stop;
    }

    return direction;
}

manual_method URI::ManualMethodComboBox2Method()
{
    int manualMethodIndex = ui->comboBox_manualMethod->currentIndex();
    switch(manualMethodIndex){
    case 0:
        return mm_jog;
    case 1:
        return mm_con;
    default:
        PRINTF(LOG_ERR, "%s: invalid manual method index=[%d]\n", __func__, manualMethodIndex);
        return mm_con;
    }
}

int URI::ManualDirection2DofIndex(manual_direction direction)
{
    switch(direction){
    case md_xplus://X
    case md_xminus:
        return 0;
    case md_yplus://Y
    case md_yminus:
        return 1;
    case md_zplus://Z
    case md_zminus:
        return 2;
    case md_rxplus://RX
    case md_rxminus:
        return 3;
    case md_ryplus://RY
    case md_ryminus:
        return 4;
    case md_rzplus://RZ
    case md_rzminus:
        return 5;
    case md_stop://释放按钮出现
        return 0;
    default:
        PRINTF(LOG_ERR, "%s: unvalid direction(%d).\n", __func__, direction);
        return 0;
    }
}

double URI::ManualSpeedSlider2Speed(manual_direction direction)
{
    int speedLevel = ui->verticalSlider_manualSpeed->value();
    int dofIndex = ManualDirection2DofIndex(direction);
    Q_ASSERT(dofIndex < RobotParams::dof);

    double manualSpeed = speedLevel * conf->manualSpeedRatio[dofIndex];
    return manualSpeed;
}

void URI::ResetActionWidget()
{
    ui->pushButton_pause->setText(QObject::tr("暂停"));
    ui->pushButton_startToWork->setEnabled(false);
    ui->pushButton_startArc->setEnabled(true);
    ui->pushButton_chooseSection->setEnabled(true);
    ui->pushButton_continous->setEnabled(true);
    ui->pushButton_origin->setEnabled(true);
}

void URI::DisableTeachWidgets()
{
    ui->pushButton_line1->setEnabled(false);
    ui->pushButton_line2->setEnabled(false);
    ui->pushButton_startTeach->setEnabled(true);
    ui->pushButton_stopTeach->setEnabled(false);
    ui->pushButton_trans->setEnabled(false);
    ui->pushButton_joint->setEnabled(false);
    ui->pushButton_suspend->setEnabled(false);
    ui->pushButton_plc->setEnabled(false);
    ui->pushButton_arc1->setEnabled(false);
    ui->pushButton_arc2->setEnabled(false);
    ui->pushButton_arc3->setEnabled(false);
    ui->pushButton_dot->setEnabled(false);
    ui->lineEdit_dotTime->setEnabled(false);
    ui->pushButton_conFirst->setEnabled(false);
    ui->pushButton_conNext->setEnabled(false);
    ui->pushButton_conFinish->setEnabled(false);
}

void URI::ResetTeachWidgetVibration()
{
    ui->checkBox_v1->setEnabled(true);
    ui->checkBox_v2->setEnabled(true);
    ui->checkBox_v3->setEnabled(true);
    ui->lineEdit_weldSpeed->setEnabled(true);

    if(ui->checkBox_v1->isChecked()){
        ui->lineEdit_frequencyFB->setEnabled(true);
        ui->lineEdit_amplitudeFB->setEnabled(true);
        ui->lineEdit_frequencyLR->setEnabled(true);
        ui->lineEdit_amplitudeLR->setEnabled(true);
        ui->comboBox_vibrationType->setEnabled(true);
    }
}

void URI::UpdateTeachWidgetSectionList()
{
    std::vector<char> sectionModeList = mTeach->GetSectionsList();

    ui->listWidget_sections->clear();
    for(size_t i=0; i<sectionModeList.size(); ++i){
        QString sectionText = QString::number(i+1) + ":";
        char sectionMode = sectionModeList[i];
        switch(sectionMode){
        case 'R':
            sectionText += QObject::tr("配置段");
            break;
        case 'Q':
            sectionText += QObject::tr("等待点");
            break;
        case 'T':
            sectionText += QObject::tr("过渡点");
            break;
        case 'L':
            sectionText += QObject::tr("直线段");
            break;
        case 'D':
            sectionText += QObject::tr("点段");
            break;
        case 'S':
            sectionText += QObject::tr("连续段");
            break;
        case 'C':
            sectionText += QObject::tr("弧线段");
            break;
        case 'O':
            sectionText += QObject::tr("整圆段");
            break;
        case 'J':
            sectionText += QObject::tr("连接符");
            break;
        case 'P':
            sectionText += QObject::tr("逻辑段");
            break;
        case 'E':
            sectionText += QObject::tr("椭弧段");
            break;
        case 'F':
            sectionText += QObject::tr("整椭段");
            break;
        case 'I':
            sectionText += QObject::tr("相贯段");
            break;
        default:
            sectionText += QObject::tr("未知段");
            break;
        }
        ui->listWidget_sections->addItem(sectionText);
    }

    char lastSectionMode = sectionModeList.empty() ? 0 : sectionModeList.back();
    switch(lastSectionMode){
    case 'L':
    case 'S':
    case 'C':
    case 'O':
    case 'D':
    case 'E':
    case 'F':
    case 'I':
        ui->pushButton_joint->setEnabled(true);
        break;
    default:
        ui->pushButton_joint->setEnabled(false);
        break;
    }

    ui->pushButton_delete->setEnabled(false);
    ui->pushButton_addSection->setEnabled(false);
    ui->pushButton_attribute->setEnabled(true);
    ui->pushButton_undo->setEnabled( mTeach->GetUndoListCount()>0 );
    ui->pushButton_redo->setEnabled( mTeach->GetRedoListCount()>0 );
    if(mTeach->GetTeachStatus() != Teach::INSERT_TEACH){
        ui->pushButton_addSection->setText(QObject::tr("插入\n示教"));
    }
}

void URI::InitSignalSlot()
{
    connect(&m_logicUpdateTimer, SIGNAL(timeout()), this, SLOT(ss_on_logicUpdateTimer_timeout()));
    connect(&m_widgetUpdateTimer, SIGNAL(timeout()), this, SLOT(ss_on_widgetUpdateTimer_timeout()));

    connect(ui->listWidget_settings, SIGNAL(currentItemChanged(QListWidgetItem*,QListWidgetItem*)), this, SLOT(on_updateSettingWidgets(QListWidgetItem*,QListWidgetItem*)));
}

void URI::InitWidgetDisplay()
{
    ui->tab1->setEnabled(false);
    ui->tab2->setEnabled(false);
    ui->tab3->setEnabled(false);
    ui->tab12->setEnabled(false);

    InitManualControlButtonWidgets();
    InitSingleAxisWidgets();
    DisableTeachWidgets();
}

void URI::InitSingleAxisWidgets()
{
    //隐藏单轴页面中多余的轴
    switch(RobotParams::axisNum){
    case 1:
        HideSingleAxisWidget(2);
        //fall through
    case 2:
        HideSingleAxisWidget(3);
        //fall through
    case 3:
        HideSingleAxisWidget(4);
        //fall through
    case 4:
        HideSingleAxisWidget(5);
        //fall through
    case 5:
        HideSingleAxisWidget(6);
    case 6:
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

void URI::InitManualControlButtonWidgets()
{
    ui->comboBox_manualMethod->setCurrentIndex(1);//初始联动

    switch (RobotParams::dof){
    case 3:
        ui->pushButton_manualControlRXMinus->hide();
        ui->pushButton_manualControlRXPlus->hide();
        //no break!
    case 4:
        ui->pushButton_manualControlRYMinus->hide();
        ui->pushButton_manualControlRYPlus->hide();
        //no break!
    case 5:
        ui->pushButton_manualControlRZMinus->hide();
        ui->pushButton_manualControlRZPlus->hide();
        //no break!
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

void URI::UpdateWidgetByThetaMatrix()
{
    double robotTheta[RobotParams::axisNum];
    for(int i=0; i<RobotParams::axisNum; ++i){
        robotTheta[i] = rbtThetaMatrix->GetTheta(i);
    }
    double robotMatrix[RobotParams::dof];
    for(int i=0; i<RobotParams::dof; ++i){
        robotMatrix[i] = rbtThetaMatrix->GetMatrix(i);
    }

    //theta
    QVector<QLineEdit*> thetaLineEdits;
    thetaLineEdits.push_back(ui->lineEdit_theta1);
    thetaLineEdits.push_back(ui->lineEdit_theta2);
    thetaLineEdits.push_back(ui->lineEdit_theta3);
    thetaLineEdits.push_back(ui->lineEdit_theta4);
    thetaLineEdits.push_back(ui->lineEdit_theta5);
    thetaLineEdits.push_back(ui->lineEdit_theta6);
    for(int i=0; i<RobotParams::axisNum; ++i){
        QLineEdit* lineEdit = thetaLineEdits[i];
        double value = robotTheta[i];
        MyWidgetHelper::SetLineEditValue(lineEdit, value);
    }

    //matrix
    const int matrixNum = 3;
    QVector<QLineEdit*> matrixLineEdits;
    matrixLineEdits.push_back(ui->lineEdit_x);
    matrixLineEdits.push_back(ui->lineEdit_y);
    matrixLineEdits.push_back(ui->lineEdit_z);
    for(int i=0; i<matrixNum; ++i){
        QLineEdit* lineEdit = matrixLineEdits[i];
        double value = robotMatrix[i];
        MyWidgetHelper::SetLineEditValue(lineEdit, value);
    }
}

void URI::UpdateWidgetByActionMsg()
{
    const ActionMsg& actionMsg = m_unifiedRobot->GetActionMsg();

    if(actionMsg.status == ActionStatus::RUNNING)//示教运行中
    {
        if(actionMsg.isGunOn){
            ui->lineEdit_gunState->setText(QObject::tr("打开"));
            ui->lineEdit_speed->setText(QString::number(actionMsg.weldSpeed));
            ui->pushButton_minusSpeed->setEnabled(true);
            ui->pushButton_plusSpeed->setEnabled(true);
        }else{
            ui->lineEdit_gunState->setText(QObject::tr("关闭"));
            ui->lineEdit_speed->setText(QObject::tr("未示教"));
            ui->pushButton_minusSpeed->setEnabled(false);
            ui->pushButton_plusSpeed->setEnabled(false);
        }
        ui->lineEdit_actionStatus->setText(QObject::tr("进行中.."));

        QString actionModeString;
        switch(actionMsg.mod){
        case 'L':
            actionModeString = QObject::tr("直线段");
            break;
        case 'D':
            actionModeString = QObject::tr("点段");
            break;
        case 'T':
            actionModeString = QObject::tr("过渡段");
            break;
        case 'S':
            actionModeString = QObject::tr("连续段");
            break;
        case 'C':
            actionModeString = QObject::tr("弧线段");
            break;
        case 'J':
            actionModeString = QObject::tr("连接符");
            break;
        case 'O':
            actionModeString = QObject::tr("整圆段");
            break;
        case 'Q':
            actionModeString = QObject::tr("等待段");
            break;
        case 'P':
            actionModeString = QObject::tr("逻辑段");
            break;
        case 'E':
            actionModeString = QObject::tr("椭圆段");
            break;
        case 'F':
            actionModeString = QObject::tr("整椭段");
            break;
        case 'I':
            actionModeString = QObject::tr("相贯段");
            break;
        default:
            actionModeString = QObject::tr("未定义");
            break;
        }
        ui->lineEdit_sectionType->setText(actionModeString);

        QString timesString = QString::number(actionMsg.nowTimes) + "/" + QString::number(actionMsg.loopTimes);
        ui->lineEdit_times->setText(timesString);
        ui->lineEdit_sectionNo->setText(QString::number(actionMsg.sectionNo));

        bool enablePauseFlag;
        switch (actionMsg.mod){
        case 'R':
        case 'L':
        case 'D':
        case 'T':
        case 'S':
        case 'C':
        case 'E':
        case 'F':
        case 'I':
        case 'J':
        case 'P':
        case 'O':
            enablePauseFlag = true;
            break;
        case 'Q':
            enablePauseFlag = false;
            break;
        default:
            PRINTF(LOG_ERR, "%s: invalid mode!\n", __func__);
            return;
        }
        ui->pushButton_pause->setEnabled(enablePauseFlag);
    }
    else//未示教
    {
        if(actionMsg.pausedNo > 0){
            ui->lineEdit_actionStatus->setText(QObject::tr("暂停"));
            QString timesString = QString::number(actionMsg.nowTimes) + "/" + QString::number(actionMsg.loopTimes);
            ui->lineEdit_times->setText(timesString);
            ui->pushButton_pause->setEnabled(true);
            ui->lineEdit_sectionNo->setText(QString::number(actionMsg.pausedNo));
        }else{
            ui->lineEdit_actionStatus->setText(QObject::tr("停止"));
            ui->lineEdit_times->setText(QObject::tr("未示教"));
            ui->pushButton_pause->setEnabled(false);
            ui->lineEdit_sectionNo->setText(QObject::tr("未示教"));
        }
        ui->lineEdit_sectionType->setText(QObject::tr("未示教"));
    }
}

void URI::UpdateWidgetByCmtConnection()
{
    bool isCmtConnected = m_unifiedRobot->GetIsCmtConnected();
    if( !ui->tab1->isEnabled() && isCmtConnected ){
        ui->tab1->setEnabled(true);
    }else if( !isCmtConnected ){
        ui->tab1->setEnabled(false);
        ui->tab2->setEnabled(false);
        ui->tab3->setEnabled(false);
    }
}

void URI::RegisterUpdateActionCallback() // 界面显示解除绑定
{
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateStopWeld, std::tr1::bind(&URI::On_UpdateStopWeld, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateMessageInform, std::tr1::bind(&URI::On_UpdateMessageInform, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateUnifiedInform, std::tr1::bind(&URI::On_UpdateUnifiedInform, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateGoHomeResult, std::tr1::bind(&URI::On_UpdateGoHomeResult, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateOutControlStatus, std::tr1::bind(&URI::On_UpdateOutControlStatus, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateInSwitchStatus, std::tr1::bind(&URI::On_UpdateInSwitchStatus, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateReInit, std::tr1::bind(&URI::on_UpdateReInit, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateFinishAction, std::tr1::bind(&URI::On_UpdateFinishAction, this));
    m_unifiedRobot->RegisterActionCallback(Callback_UpdateAtSuspendPoint, std::tr1::bind(&URI::On_UpdateAtSuspendPoint, this));
}

void URI::On_UpdateGoHomeResult()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    int goHomeResult = m_unifiedRobot->GetGoHomeResult();
    DisplayWidget_FinishGoHome( goHomeResult );
}

void URI::On_UpdateStopWeld()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);
}

void URI::On_UpdateOutControlStatus()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

#ifndef NVH_USED
    int outStatus = m_unifiedRobot->GetOutCtrlStatus();

    const int outCtrolNum = 5;
    QVector<QPushButton*> outCtrlButtons;
    outCtrlButtons.push_back(ui->pushButton_outCtrl1);
    outCtrlButtons.push_back(ui->pushButton_outCtrl2);
    outCtrlButtons.push_back(ui->pushButton_outCtrl3);
    outCtrlButtons.push_back(ui->pushButton_outCtrl4);
    outCtrlButtons.push_back(ui->pushButton_outCtrl5);
    QVector<QLineEdit*> outCtrlLineEdits;
    outCtrlLineEdits.push_back(ui->lineEdit_outCtrl1);
    outCtrlLineEdits.push_back(ui->lineEdit_outCtrl2);
    outCtrlLineEdits.push_back(ui->lineEdit_outCtrl3);
    outCtrlLineEdits.push_back(ui->lineEdit_outCtrl4);
    outCtrlLineEdits.push_back(ui->lineEdit_outCtrl5);

    for(int i=0; i<outCtrolNum; ++i){
        bool onOffStatus = outStatus & (1<<i);

        QPushButton* outCtrlbtn = outCtrlButtons[i];
        QLineEdit* outCtrlLineEdit = outCtrlLineEdits[i];
        MyWidgetHelper::SetOutCtrlButtonStatus(outCtrlbtn, onOffStatus);
        MyWidgetHelper::SetOutCtrlLineEditStatus(outCtrlLineEdit, onOffStatus);
    }
#endif
}

void URI::On_UpdateInSwitchStatus()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

#ifndef NVH_USED
    int inStatus = m_unifiedRobot->GetInSwitchStatus();

    const int inSwitchNum = 6;
    QVector<QLineEdit*> inSwitchLineEdits;
    inSwitchLineEdits.push_back(ui->lineEdit_inSignal1);
    inSwitchLineEdits.push_back(ui->lineEdit_inSignal2);
    inSwitchLineEdits.push_back(ui->lineEdit_inSignal3);
    inSwitchLineEdits.push_back(ui->lineEdit_inSignal4);
    inSwitchLineEdits.push_back(ui->lineEdit_inSignal5);
    inSwitchLineEdits.push_back(ui->lineEdit_inSignal6);

    for(int i=0; i<inSwitchNum; ++i){
        bool onOffStatus = inStatus & (1<<i);

        QLineEdit* inSwitchLineEdit = inSwitchLineEdits[i];
        MyWidgetHelper::SetInSwitchLineEditStatus(inSwitchLineEdit, onOffStatus);
    }
#endif

    UpdateInSwitchStatus_MedicalUltrasoundRobot();
}

void URI::On_UpdateMessageInform()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    bool enableShowInformBox = MiscConfigurationParam::Instance()->GetConfigParam(MiscConfigurationParam::eEnableShowBoxWhenReceiveMessageInform);
    const MsgStructure::MessageInform& messageInform = m_unifiedRobot->GetMessageInform();

    switch (messageInform.informType) {
    case MsgStructure::MessageInform::MedicalAxis4Angle:
        emit Signal_MedicalUltrasoundRobotAxis4Theta();
        break;
    case MsgStructure::MessageInform::EmergencyStop:
        emit Signal_RobotEmergencyStop(true);

        if(enableShowInformBox){
#ifdef NVH_USED
            emit Signal_WidgetShowInformMessageBox("机器人急停中...");
#else
            MyMessageBox::ShowInformMessageBox("机器人急停中...");
#endif
        }
        break;
    case MsgStructure::MessageInform::EmergencyStopError:
        emit Signal_RobotEmergencyStop(false);
        PRINTF(LOG_DEBUG, "%s:emergency stop error.\n", __func__);

        if(enableShowInformBox){
#ifdef NVH_USED
            emit Signal_WidgetShowInformMessageBox("机器人已停止运动!\n注意! 由于尚未回原,踏板不会上抬!");
#else
            MyMessageBox::ShowInformMessageBox("机器人已停止运动!\n注意! 由于尚未回原,踏板不会上抬!");
#endif
        }
        break;
    default:
        PRINTF(LOG_INFO, "%s: undefined inform type\n",__func__);
        break;
    }
}

void URI::On_UpdateUnifiedInform()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);
}

void URI::on_UpdateReInit()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

#ifndef NVH_USED
    DisableTeachWidgets();
#endif

    m_unifiedRobot->SendConfiguration();
}

void URI::On_UpdateFinishAction()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

#ifndef NVH_USED
    ResetActionWidget();
#endif
}

void URI::On_UpdateAtSuspendPoint()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

#ifndef NVH_USED
    DisplayWidget_AtSuspendPoint();
#endif
}

void URI::ss_on_logicUpdateTimer_timeout()
{
    UpdateLogic();
}

void URI::ss_on_widgetUpdateTimer_timeout()
{
    UpdateWidgets();
}

void URI::on_pushButton_origin_clicked()
{
    m_unifiedRobot->SendEnterIdleMsg();//回原前 停止机器人

    bool flag = MyMessageBox::ShowQuestionMessageBox("返回原点时请勿进行其他操作!\n开始回原?");
    if(!flag){
        PRINTF(LOG_DEBUG, "%s is canceled\n",__func__);
        return;
    }

    PRINTF(LOG_DEBUG, "%s starts\n", __func__);
    m_unifiedRobot->SendGoHomeMsg();

    EnableGoHomeWidgets(false);
}

void URI::on_pushButton_softstop_clicked()
{
    ui->tab13->setEnabled(true);
    ResetSingleAxisWidgets();
    ResetActionWidget();

    m_unifiedRobot->SendEnterIdleMsg();
}

void URI::ss_on_pushButton_manualControlN_pressed()
{
    QPushButton *btn = qobject_cast<QPushButton*>( sender() );
    if(!btn){
        PRINTF(LOG_ERR, "%s: cannot cast btn.\n", __func__);
        return;
    }
    if(ManualMethodComboBox2Method() != mm_con){
        return;
    }

    manual_direction direction = ManualControlButton2Direction(btn);
    double manualSpeed = ManualSpeedSlider2Speed(direction);
    m_unifiedRobot->SendManualControlMsg(mm_con, direction, manualSpeed);
}

void URI::ss_on_pushButton_manualControlN_released()
{
    if(ManualMethodComboBox2Method() != mm_con){
        return;
    }

    m_unifiedRobot->SendManualControlMsg(mm_jog, md_stop, 0.0);
}

void URI::ss_on_pushButton_manualControlN_clicked()
{
    QPushButton *btn = qobject_cast<QPushButton*>( sender() );
    if(!btn){
        PRINTF(LOG_ERR, "%s: cannot cast btn.\n", __func__);
        return;
    }
    if(ManualMethodComboBox2Method() != mm_jog){
        return;
    }

    manual_direction direction = ManualControlButton2Direction(btn);
    double manualSpeed = ManualSpeedSlider2Speed(direction);
    m_unifiedRobot->SendManualControlMsg(mm_jog, direction, manualSpeed);
}

void URI::ss_on_horizontalSlider_singleAxisN_valueChanged(int value)
{
    QSlider* slider = qobject_cast<QSlider*>(this->sender());
    if(slider == Q_NULLPTR){
        PRINTF(LOG_ERR, "%s: invalid slider!\n", __func__);
        return;
    }

    Q_UNUSED(value);
    int singleAxisIndex = SingleAxisSlider2AxisIndex(slider);
    double singleAxisSpeed = SingleAxisSlider2Speed(slider);
    m_unifiedRobot->SendSingleAxisMsg(singleAxisIndex, singleAxisSpeed);
}

void URI::ss_on_pushButton_stopAxisN_pressed()
{
    QPushButton* button = qobject_cast<QPushButton*>(this->sender());
    if(button == Q_NULLPTR){
        PRINTF(LOG_ERR, "%s: invalid slider!\n", __func__);
        return;
    }

    int axisIndex = SingleAxisButton2AxisIndex(button);
    if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: axisIndex(%d) out of range.\n", __func__, axisIndex);
        return;
    }
    int direction = SingleAxisButton2Direction(button);
    double speed = direction * conf->singleAxisBtnRatio[axisIndex];

    m_unifiedRobot->SendSingleAxisMsg(axisIndex, speed);
}

void URI::ss_on_pushButton_stopAxisN_released()
{
    QPushButton* button = qobject_cast<QPushButton*>(this->sender());
    if(button == Q_NULLPTR){
        PRINTF(LOG_ERR, "%s: invalid slider!\n", __func__);
        return;
    }

    int axisIndex = SingleAxisButton2AxisIndex(button);
    if(axisIndex<0 || axisIndex>=RobotParams::axisNum){
        PRINTF(LOG_ERR, "%s: axisIndex(%d) out of range.\n", __func__, axisIndex);
        return;
    }

    m_unifiedRobot->SendSingleAxisMsg(axisIndex, 0.0);
}

void URI::ss_on_pushButton_stopAxisN_clicked()
{
    ResetSingleAxisWidgets();
    m_unifiedRobot->SendEnterIdleMsg();
}

void URI::ss_on_pushButton_outCtrlN_clicked()
{
    int outCtrlStatus = 0;
    if(ui->pushButton_outCtrl1->isChecked()){
        outCtrlStatus |= (1<<0);
    }
    if(ui->pushButton_outCtrl2->isChecked()){
        outCtrlStatus |= (1<<1);
    }
    if(ui->pushButton_outCtrl3->isChecked()){
        outCtrlStatus |= (1<<2);
    }
    if(ui->pushButton_outCtrl4->isChecked()){
        outCtrlStatus |= (1<<3);
    }
    if(ui->pushButton_outCtrl5->isChecked()){
        outCtrlStatus |= (1<<4);
    }

    m_unifiedRobot->SendOutControlCmd(outCtrlStatus);
    PRINTF(LOG_DEBUG, "%s: outCtrlStatus=[%d]\n", __func__, outCtrlStatus);
}

void URI::on_pushButton_chooseFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(Q_NULLPTR, "Choose file", conf->GetFilePath(Configuration::RootFolderPath).c_str());
    if(filePath.isEmpty()){
        return;
    }
    if( !QFile::exists(filePath) ){
        MyMessageBox::ShowInformMessageBox("所选定的示教文件不存在，请重新选择!");
        return;
    }

    conf->defaultFile = filePath.toStdString();
    conf->SaveToFile();
}

void URI::on_pushButton_startArc_clicked()
{
    std::string questionContent = std::string("确认进行示教?\n") + conf->defaultFile;
    bool confirmFlag = MyMessageBox::ShowQuestionMessageBox(questionContent);
    if(!confirmFlag){
        return;
    }

    const std::string& teachFilePath = conf->defaultFile;
    bool startOk = m_unifiedRobot->StartActionByTeachFile(teachFilePath);
    if(!startOk){
        MyMessageBox::ShowInformMessageBox("动作失败!");
        return;
    }
}

void URI::on_pushButton_stopWeld_clicked()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    ui->pushButton_pause->setText(QObject::tr("暂停"));
    ui->pushButton_startToWork->setEnabled(false);

    m_unifiedRobot->SendEnterIdleMsg();
}

void URI::on_pushButton_chooseSection_clicked()
{
}

void URI::on_pushButton_continous_clicked()
{
    bool ok = false;
    int loopTimes = QInputDialog::getInt(this, "Scara", QObject::tr("输入连续次数"), 1, 1, 90000000, 1, &ok);
    if(!ok || loopTimes<=0){
        PRINTF(LOG_ERR, "%s: invalid params!\n", __func__);
        return;
    }

    const std::string& teachFilePath = conf->defaultFile;
    bool startOk = m_unifiedRobot->StartActionByTeachFile(teachFilePath);
    if(!startOk){
        MyMessageBox::ShowInformMessageBox("动作失败!");
        return;
    }
}

void URI::on_pushButton_startToWork_clicked()
{
    ui->pushButton_startToWork->setEnabled(false);
}

void URI::on_pushButton_pause_clicked()
{
    QString pauseButtonText;
    bool isPausedFlag;
    if(m_unifiedRobot->GetActionMsg().status == ActionStatus::RUNNING){
        pauseButtonText = QObject::tr("继续");
        isPausedFlag = true;
    }else{
        pauseButtonText = QObject::tr("暂停");
        isPausedFlag = false;
    }

    ui->pushButton_pause->setText(pauseButtonText);
    m_unifiedRobot->SendActionPauseResumeMsg(isPausedFlag);
}

void URI::on_pushButton_startTeach_clicked()
{
    if(m_unifiedRobot->GetActionMsg().status == ActionStatus::RUNNING)
    {
        bool stopRobotFlag = MyMessageBox::ShowQuestionMessageBox("停止动作后才能进行示教!\n是否停止示教动作?");
        if(!stopRobotFlag){
            return;
        }
        m_unifiedRobot->SendEnterIdleMsg();
    }

    if(mTeach->StartTeach()){//success
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_startTeach->setEnabled(false);
        ui->pushButton_stopTeach->setEnabled(true);
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_editTeach->setEnabled(false);
        ui->pushButton_homeTimes->setEnabled(false);
        ResetTeachWidgetVibration();
        ui->lineEdit_weldSpeed->setText("12");
        ui->lineEdit_frequencyFB->setText("0");
        ui->lineEdit_frequencyLR->setText("0");
        ui->lineEdit_amplitudeFB->setText("0");
        ui->lineEdit_amplitudeLR->setText("0");
    }
}

void URI::on_pushButton_stopTeach_clicked()
{
    mTeach->StopTeach();

    DisableTeachWidgets();
    UpdateTeachWidgetSectionList();
    ui->pushButton_undo->setEnabled(false);
    ui->pushButton_redo->setEnabled(false);
    ui->pushButton_editTeach->setEnabled(true);
    ui->pushButton_homeTimes->setEnabled(false);
    ui->checkBox_v1->setEnabled(false);
    ui->checkBox_v2->setEnabled(false);
    ui->checkBox_v3->setEnabled(false);
    ui->lineEdit_weldSpeed->setEnabled(false);
    ui->lineEdit_frequencyFB->setEnabled(false);
    ui->lineEdit_amplitudeFB->setEnabled(false);
    ui->lineEdit_frequencyLR->setEnabled(false);
    ui->lineEdit_amplitudeLR->setEnabled(false);
    ui->comboBox_vibrationType->setEnabled(false);
}

void URI::on_pushButton_trans_clicked()
{
    mTeach->RecordTransSection();

    ui->pushButton_joint->setEnabled(false);
    if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
    {
        ui->pushButton_stopTeach->setEnabled(true);
    }
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_suspend_clicked()
{
    mTeach->RecordSuspendSection();

    ui->pushButton_joint->setEnabled(false);
    if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
    {
        ui->pushButton_stopTeach->setEnabled(true);
    }
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_joint_clicked()
{
    mTeach->RecordJointSection();
}

void URI::on_pushButton_plc_clicked()
{
    mTeach->RecordPlcSection();

    if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
    {
        ui->pushButton_stopTeach->setEnabled(true);
    }
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_delete_clicked()
{
    int pos=ui->listWidget_sections->currentRow();
    mTeach->DeleteSection(pos);

    UpdateTeachWidgetSectionList();
    pos=std::max(pos,ui->listWidget_sections->count()-1);
    ui->listWidget_sections->setCurrentRow(pos);
}

void URI::on_pushButton_addSection_clicked()
{
    int insertIndex = ui->listWidget_sections->currentRow();
    if(mTeach->GetTeachStatus()!=Teach::INSERT_TEACH)
    {
        StatusString::Instance()->SetStatusIndex(StatusString::TeachStatus_InsertTeachSection);
        mTeach->InsertTeach(insertIndex);

        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_joint->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        //PushButtonKeyPoint1->setEnabled(true);
        //PushButtonKeyPoint2->setEnabled(false);
        //PushButtonIntersection1->setEnabled(false);
        //PushButtonIntersection2->setEnabled(false);
        //PushButtonIntersection3->setEnabled(false);
        ui->pushButton_addSection->setText(QObject::tr("取消\n插入"));
    }else{
        StatusString::Instance()->SetStatusIndex(StatusString::TeachStatus_CancelInsertTeachSection);
        mTeach->CancelInsertTeach();
        ui->pushButton_addSection->setText(QObject::tr("插入\n示教"));
        UpdateTeachWidgetSectionList();
    }
    ui->listWidget_sections->setCurrentRow(insertIndex);
}

void URI::on_pushButton_attribute_clicked()
{
    int pos = ui->listWidget_sections->currentRow();
    SectionInfo* si= mTeach->GetSectionInfo(pos);
    if(si == NULL){
        MyMessageBox::ShowInformMessageBox("请先选择要设置的段");
        return;
    }

    //si指向选择的对象
    switch(si->GetMode()){
    case 'T':{/*{}限定作用域 及时释放栈上内容*/
        TSectionInfo *si2=dynamic_cast<TSectionInfo*>(si);
        SectionAttributeDialog dialog(si->GetMode(),&si2->transSpeed);
        dialog.exec();}
        break;
    case 'D':{
        DSectionInfo *si2=dynamic_cast<DSectionInfo*>(si);
        SectionAttributeDialog dialog(si->GetMode(),&si2->transSpeed,&si2->dotTime);
        dialog.exec();}
        break;
    case 'L':{
        LSectionInfo *si2=dynamic_cast<LSectionInfo*>(si);
        SectionAttributeDialog dialog(si->GetMode(),&si2->transSpeed,0,&si2->weldSpeed,&si2->vibType,
                                      &si2->vibrationFreqFB,&si2->vibrationAmplFB,
                                      &si2->vibrationFreqLR,&si2->vibrationAmplLR);
        dialog.exec();}
        break;
    case 'S':{
        SSectionInfo *si2=dynamic_cast<SSectionInfo*>(si);
        SectionAttributeDialog dialog(si->GetMode(),&si2->transSpeed,0,&si2->weldSpeed,&si2->vibType,
                                      &si2->vibrationFreqFB,&si2->vibrationAmplFB,
                                      &si2->vibrationFreqLR,&si2->vibrationAmplLR);
        dialog.exec();}
        break;
    case 'C':
    case 'O':{
        CSectionInfo *si2=dynamic_cast<CSectionInfo*>(si);
        SectionAttributeDialog dialog(si->GetMode(),&si2->transSpeed,0,&si2->weldSpeed,&si2->vibType,
                                      &si2->vibrationFreqFB,&si2->vibrationAmplFB,
                                      &si2->vibrationFreqLR,&si2->vibrationAmplLR);
        dialog.exec();}
        break;
    }
}

void URI::on_pushButton_undo_clicked()
{
    mTeach->UndoTeach();
    ui->pushButton_redo->setEnabled(true);
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_redo_clicked()
{
    mTeach->RedoTeach();
    ui->pushButton_undo->setEnabled(true);
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_line1_clicked()
{
    double weldSpeed=ui->lineEdit_weldSpeed->text().toDouble();

    double vibrationFreqFB=ui->lineEdit_frequencyFB->text().toDouble();
    double vibrationAmplFB=ui->lineEdit_amplitudeFB->text().toDouble();
    double vibrationFreqLR=ui->lineEdit_frequencyLR->text().toDouble();
    double vibrationAmplLR=ui->lineEdit_amplitudeLR->text().toDouble();
    int vibType=0;
    if(ui->checkBox_v1->isChecked()){
        vibType=ui->comboBox_vibrationType->currentIndex()+1;
    }

    if(mTeach->RecordLineSectionStartPoint(weldSpeed,vibType,vibrationFreqFB,vibrationAmplFB,vibrationFreqLR,vibrationAmplLR) == 0){//success
        ui->pushButton_conFirst->setEnabled(false);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(false);
        ui->pushButton_line2->setEnabled(true);
        ui->pushButton_trans->setEnabled(false);
        ui->pushButton_joint->setEnabled(false);
        ui->pushButton_suspend->setEnabled(false);
        ui->pushButton_plc->setEnabled(false);
        ui->pushButton_arc1->setEnabled(false);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(false);
        ui->lineEdit_dotTime->setEnabled(false);
        ui->pushButton_stopTeach->setEnabled(false);
        ui->pushButton_clear1->setEnabled(true);
        //PushButtonKeyPoint1->setEnabled(false);
        //PushButtonKeyPoint2->setEnabled(false);
        //PushButtonIntersection1->setEnabled(false);
        //PushButtonIntersection2->setEnabled(false);
        //PushButtonIntersection3->setEnabled(false);

        ui->checkBox_v1->setEnabled(false);
        ui->checkBox_v2->setEnabled(false);
        ui->checkBox_v3->setEnabled(false);
        ui->lineEdit_weldSpeed->setEnabled(false);
        ui->lineEdit_frequencyFB->setEnabled(false);
        ui->lineEdit_amplitudeFB->setEnabled(false);
        ui->lineEdit_frequencyLR->setEnabled(false);
        ui->lineEdit_amplitudeLR->setEnabled(false);
        ui->comboBox_vibrationType->setEnabled(false);
    }
}

void URI::on_pushButton_line2_clicked()
{
    if(mTeach->RecordLineSectionEndPoint() == 0){//success
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_joint->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->pushButton_clear1->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(true);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
        {
            ui->pushButton_stopTeach->setEnabled(true);
        }
        UpdateTeachWidgetSectionList();
        ResetTeachWidgetVibration();
    }
}

void URI::on_pushButton_clear1_clicked()//示教-直线-后退
{
    mTeach->ClearCurrentSection();

    ui->pushButton_conFirst->setEnabled(true);
    ui->pushButton_conNext->setEnabled(false);
    ui->pushButton_conFinish->setEnabled(false);
    ui->pushButton_line1->setEnabled(true);
    ui->pushButton_line2->setEnabled(false);
    ui->pushButton_trans->setEnabled(true);
    ui->pushButton_suspend->setEnabled(true);
    ui->pushButton_plc->setEnabled(true);
    ui->pushButton_arc1->setEnabled(true);
    ui->pushButton_arc2->setEnabled(false);
    ui->pushButton_arc3->setEnabled(false);
    ui->pushButton_dot->setEnabled(true);
    ui->lineEdit_dotTime->setEnabled(true);
    ui->pushButton_clear1->setEnabled(false);
//    PushButtonKeyPoint1->setEnabled(true);
//    PushButtonKeyPoint2->setEnabled(false);
//    PushButtonIntersection1->setEnabled(false);
//    PushButtonIntersection2->setEnabled(false);
//    PushButtonIntersection3->setEnabled(false);
    if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
    {
        ui->pushButton_stopTeach->setEnabled(true);
    }
    ResetTeachWidgetVibration();
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_arc1_clicked()
{
    double weldSpeed=ui->lineEdit_weldSpeed->text().toDouble();

    double vibrationFreqFB=ui->lineEdit_frequencyFB->text().toDouble();
    double vibrationAmplFB=ui->lineEdit_amplitudeFB->text().toDouble();
    double vibrationFreqLR=ui->lineEdit_frequencyLR->text().toDouble();
    double vibrationAmplLR=ui->lineEdit_amplitudeLR->text().toDouble();
    int vibType=0;
    if(ui->checkBox_v2->isChecked()){
        vibType=ui->comboBox_vibrationType->currentIndex()+1;
    }

    if(mTeach->RecordCircleSectionPoint1(weldSpeed,vibType,vibrationFreqFB,vibrationAmplFB,vibrationFreqLR,vibrationAmplLR)==0)
    {
        ui->pushButton_conFirst->setEnabled(false);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(false);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(false);
        ui->pushButton_joint->setEnabled(false);
        ui->pushButton_suspend->setEnabled(false);
        ui->pushButton_plc->setEnabled(false);
        ui->pushButton_arc1->setEnabled(false);
        ui->pushButton_arc2->setEnabled(true);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(false);
        ui->lineEdit_dotTime->setEnabled(false);
        ui->pushButton_stopTeach->setEnabled(false);
        ui->pushButton_clear2->setEnabled(true);
//        PushButtonKeyPoint1->setEnabled(false);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        ui->checkBox_v1->setEnabled(false);
        ui->checkBox_v2->setEnabled(false);
        ui->checkBox_v3->setEnabled(false);
        ui->lineEdit_weldSpeed->setEnabled(false);
        ui->lineEdit_frequencyFB->setEnabled(false);
        ui->lineEdit_amplitudeFB->setEnabled(false);
        ui->lineEdit_frequencyLR->setEnabled(false);
        ui->lineEdit_amplitudeLR->setEnabled(false);
        ui->comboBox_vibrationType->setEnabled(false);
    }
}

void URI::on_pushButton_arc2_clicked()
{
    if(0==mTeach->RecordCircleSectionPoint2()){
        ui->pushButton_conFirst->setEnabled(false);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(false);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(false);
        ui->pushButton_joint->setEnabled(false);
        ui->pushButton_suspend->setEnabled(false);
        ui->pushButton_plc->setEnabled(false);
        ui->pushButton_arc1->setEnabled(false);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(true);
        ui->pushButton_dot->setEnabled(false);
        ui->lineEdit_dotTime->setEnabled(false);
        ui->pushButton_stopTeach->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(false);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
    }
}

void URI::on_pushButton_arc3_clicked()
{
    if(0==mTeach->RecordCircleSectionPoint3()){
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_joint->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->pushButton_clear2->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(true);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
        {
            ui->pushButton_stopTeach->setEnabled(true);
        }
        ResetTeachWidgetVibration();
        UpdateTeachWidgetSectionList();
    }
}

void URI::on_pushButton_clear2_clicked()//示教-弧线-后退
{
    if(ui->pushButton_arc2->isEnabled()==true)
    {
        mTeach->ClearCurrentSection();
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->pushButton_clear2->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(true);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
        {
            ui->pushButton_stopTeach->setEnabled(true);
        }
        ResetTeachWidgetVibration();
        UpdateTeachWidgetSectionList();
    }
    if(ui->pushButton_arc3->isEnabled()==true)
    {
        ui->pushButton_arc2->setEnabled(true);
        ui->pushButton_arc3->setEnabled(false);
    }
}

void URI::on_pushButton_conFirst_clicked()
{
    double weldSpeed=ui->lineEdit_weldSpeed->text().toDouble();

    double vibrationFreqFB=ui->lineEdit_frequencyFB->text().toDouble();
    double vibrationAmplFB=ui->lineEdit_amplitudeFB->text().toDouble();
    double vibrationFreqLR=ui->lineEdit_frequencyLR->text().toDouble();
    double vibrationAmplLR=ui->lineEdit_amplitudeLR->text().toDouble();
    int vibType=0;
    if(ui->checkBox_v3->isChecked()){
        vibType=ui->comboBox_vibrationType->currentIndex()+1;
    }

    if(mTeach->RecordSegmentSectionFirst(weldSpeed,vibType,vibrationFreqFB,vibrationAmplFB,vibrationFreqLR,vibrationAmplLR)==0){
        ui->pushButton_conFirst->setEnabled(false);
        ui->pushButton_conNext->setEnabled(true);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(false);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(false);
        ui->pushButton_joint->setEnabled(false);
        ui->pushButton_suspend->setEnabled(false);
        ui->pushButton_plc->setEnabled(false);
        ui->pushButton_arc1->setEnabled(false);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(false);
        ui->lineEdit_dotTime->setEnabled(false);
        ui->pushButton_stopTeach->setEnabled(false);
        ui->pushButton_clear3->setEnabled(true);
//        PushButtonKeyPoint1->setEnabled(false);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);

        ui->checkBox_v1->setEnabled(false);
        ui->checkBox_v2->setEnabled(false);
        ui->checkBox_v3->setEnabled(false);
        ui->lineEdit_weldSpeed->setEnabled(false);
        ui->lineEdit_frequencyFB->setEnabled(false);
        ui->lineEdit_amplitudeFB->setEnabled(false);
        ui->lineEdit_frequencyLR->setEnabled(false);
        ui->lineEdit_amplitudeLR->setEnabled(false);
        ui->comboBox_vibrationType->setEnabled(false);
    }
}

void URI::on_pushButton_conNext_clicked()
{
    int points=mTeach->RecordSegmentSectionNext();
    if(points>0){
        ui->pushButton_conFirst->setEnabled(false);
        ui->pushButton_conNext->setEnabled(true);
        ui->pushButton_conFinish->setEnabled(true);
        ui->pushButton_line1->setEnabled(false);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(false);
        ui->pushButton_joint->setEnabled(false);
        ui->pushButton_suspend->setEnabled(false);
        ui->pushButton_plc->setEnabled(false);
        ui->pushButton_arc1->setEnabled(false);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(false);
        ui->lineEdit_dotTime->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(false);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        ui->pushButton_stopTeach->setEnabled(false);
        ui->pushButton_conNext->setText(QObject::tr("添加后续点 [")+QString::number(points)+"]");
    }
}

void URI::on_pushButton_conFinish_clicked()
{
    if(0==mTeach->RecordSegmentSectionFinish()){
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_joint->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->pushButton_clear3->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(true);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
        {
            ui->pushButton_stopTeach->setEnabled(true);
        }
        ui->pushButton_conNext->setText(QObject::tr("添加后续点 [0]"));

        ResetTeachWidgetVibration();
        UpdateTeachWidgetSectionList();
    }
}

void URI::on_pushButton_clear3_clicked()//示教-连续-后退
{
    if(ui->pushButton_conNext->isEnabled()==true && mTeach->GetSegmentSectionPointsNum()==0)
    {
        mTeach->ClearCurrentSection();
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_conNext->setEnabled(false);
        ui->pushButton_conFinish->setEnabled(false);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_line2->setEnabled(false);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_arc2->setEnabled(false);
        ui->pushButton_arc3->setEnabled(false);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        ui->pushButton_clear3->setEnabled(false);
//        PushButtonKeyPoint1->setEnabled(true);
//        PushButtonKeyPoint2->setEnabled(false);
//        PushButtonIntersection1->setEnabled(false);
//        PushButtonIntersection2->setEnabled(false);
//        PushButtonIntersection3->setEnabled(false);
        if(mTeach->GetTeachStatus()!=Teach::EDIT_TEACH)
        {
            ui->pushButton_stopTeach->setEnabled(true);
        }
        ResetTeachWidgetVibration();
        UpdateTeachWidgetSectionList();
    }
    else if(ui->pushButton_conNext->isEnabled()==true && mTeach->GetSegmentSectionPointsNum()>0)
    {
        int count=mTeach->UndoRecordSegmentSection();
        if(count>=0)
            ui->pushButton_conNext->setText(QObject::tr("添加后续点 [")+QString::number(count)+"]");
    }
}

void URI::on_pushButton_dot_clicked()
{
    int dotTime=ui->lineEdit_dotTime->text().toInt();
    mTeach->RecordDotSection(dotTime);
    UpdateTeachWidgetSectionList();
}

void URI::on_pushButton_editTeach_clicked()
{
    if(mTeach->GetTeachStatus() == Teach::IDLE){
        if(m_unifiedRobot->GetActionMsg().status == ActionStatus::RUNNING){
            bool stopRobotFlag = MyMessageBox::ShowQuestionMessageBox("示教动作进程未结束!是否停止动作进程而开始示教?");
            if(!stopRobotFlag){
                return;
            }

            m_unifiedRobot->SendEnterIdleMsg();
        }

        QString editFileName = QFileDialog::getOpenFileName(Q_NULLPTR, "Choose file", conf->GetFilePath(Configuration::RootFolderPath).c_str());
        if(editFileName.isEmpty()){
            return;
        }
        if( !QFile::exists(editFileName) ){
            MyMessageBox::ShowInformMessageBox("所选定的示教文件不存在, 请重新选择!");
            return;
        }
        if(-1 == mTeach->EditTeach(editFileName.toStdString())){
            MyMessageBox::ShowInformMessageBox("无法打开示教文件!");
            return;
        }

        ui->pushButton_startTeach->setEnabled(false);
        ui->pushButton_homeTimes->setEnabled(true);
        ui->pushButton_line1->setEnabled(true);
        ui->pushButton_trans->setEnabled(true);
        ui->pushButton_suspend->setEnabled(true);
        ui->pushButton_plc->setEnabled(true);
        ui->pushButton_arc1->setEnabled(true);
        ui->pushButton_conFirst->setEnabled(true);
        ui->pushButton_dot->setEnabled(true);
        ui->lineEdit_dotTime->setEnabled(true);
        //PushButtonKeyPoint1->setEnabled(true);
        ResetTeachWidgetVibration();
        UpdateTeachWidgetSectionList();
        ui->lineEdit_weldSpeed->setText("12");
        ui->pushButton_editTeach->setText(QObject::tr("结束修改"));
    }else{
        mTeach->StopTeach();
        UpdateTeachWidgetSectionList();
        ui->pushButton_homeTimes->setEnabled(false);
        ui->pushButton_editTeach->setText(QObject::tr("修改示教"));
        DisableTeachWidgets();
        ui->pushButton_undo->setEnabled(false);
        ui->pushButton_redo->setEnabled(false);
        ui->checkBox_v1->setEnabled(false);
        ui->checkBox_v2->setEnabled(false);
        ui->checkBox_v3->setEnabled(false);
        ui->lineEdit_weldSpeed->setEnabled(false);
        ui->lineEdit_frequencyFB->setEnabled(false);
        ui->lineEdit_amplitudeFB->setEnabled(false);
        ui->lineEdit_frequencyLR->setEnabled(false);
        ui->lineEdit_amplitudeLR->setEnabled(false);
        ui->comboBox_vibrationType->setEnabled(false);
    }
}

void URI::on_updateSettingWidgets(QListWidgetItem *current, QListWidgetItem *previous)
{
    /*之前的隐藏*/
    if(settingMap.find(previous)!=settingMap.end()){
        SettingBase* sb=settingMap[previous];
        DisplaySettingWidget(sb,false);
    }
    /*现在的显示*/
    if(settingMap.find(current)!=settingMap.end()){
        SettingBase* sb=settingMap[current];
        DisplaySettingWidget(sb,true);
    }
}

void URI::on_pushButton_changePassword_clicked()
{
#ifdef NVH_USED
    if (m_settingLevel == SettingBase::RootUser)
    {
        MyMessageBox::ShowInformMessageBox("Root密码不可修改");
        return;
    }
#endif

    bool ok;
    QString password=QInputDialog::getText(this, QObject::tr("Password"), QObject::tr("请输入新密码"), QLineEdit::Normal, "", &ok);
    if(!ok){
        return;
    }
    if(password.isEmpty()){
        MyMessageBox::ShowInformMessageBox("密码必须非空");
        return;
    }

    if(m_settingLevel == SettingBase::NormalUser){
        conf->normalPassword = password.toStdString();
    }else if(m_settingLevel == SettingBase::RootUser){
        conf->rootPassword = password.toStdString();
    }

    if(conf->SaveToFile() == 0){
        MyMessageBox::ShowInformMessageBox("密码修改成功");
    }else{
        MyMessageBox::ShowInformMessageBox("密码修改失败");
    }
}

void URI::on_pushButton_saveSettings_clicked()
{
    for(std::map<QListWidgetItem*,SettingBase*>::iterator iter=settingMap.begin();iter!=settingMap.end();iter++){
        SettingBase* bs=iter->second;
        if( bs->StoreParameters(*conf) == false){
            return;
        }
    }

    if(conf->SaveToFile() == -1){
        MyMessageBox::ShowInformMessageBox("!!!设置保存失败!!!");
        return;
    }

    MyMessageBox::ShowInformMessageBox("设置保存成功");
    m_unifiedRobot->SendConfiguration();
}

void URI::on_pushButton_exitSettings_clicked()//登陆 or 退出设置
{
    if(m_settingLevel == SettingBase::UnLoggedIn){
#ifdef ENABLE_LOGIN_PASSWORD
        bool ok;
        QString password=QInputDialog::getText(this,QObject::tr("Password"),QObject::tr("请输入密码"),
                                               QLineEdit::Password,"",&ok);
        if(ok){
            if(password.toStdString() == conf->normalPassword){
                m_settingLevel = SettingBase::NormalUser;
            }else if(password.toStdString() == rootPwd){
                m_settingLevel = SettingBase::RootUser;
            }
#else
        if (conf->rootPassword == "root") m_settingLevel = SettingBase::RootUser;
        else m_settingLevel = SettingBase::NormalUser;
        {
#endif
            if(m_settingLevel >= SettingBase::NormalUser){
                ShowSettingWidgets(m_settingLevel);
                ui->pushButton_changePassword->setEnabled(true);
                ui->pushButton_saveSettings->setEnabled(true);
                ui->pushButton_exitSettings->setText(QObject::tr("退出设置"));

                // refresh
                for(std::map<QListWidgetItem*,SettingBase*>::iterator iter=settingMap.begin(); iter!=settingMap.end(); iter++){
                    SettingBase* sb = iter->second;
                    sb->LoadParameters(*conf);
                }
            }else{
                MyMessageBox::ShowInformMessageBox("密码错误!");
                return;
            }
        }
    }else{//exit settings
        HideSettingWidgets();
        m_settingLevel = SettingBase::UnLoggedIn;
        ui->pushButton_changePassword->setEnabled(false);
        ui->pushButton_saveSettings->setEnabled(false);
        ui->pushButton_exitSettings->setText(QObject::tr("登录"));
    }
}

void URI::ss_on_comboBox_currentIndexChanged(QString text)
{
    //形参QString为了适配comboBox的信号
    Q_UNUSED(text);
    PRINTF(LOG_DEBUG,"updating graph...\n");

    int sectionIndex=ui->comboBox_section->currentIndex();
    QString name=ui->comboBox_graph->currentText();
    QString axis=ui->comboBox_axisNum->currentText();
    if(axis.toInt()<1){
        ui->comboBox_axisNum->setCurrentIndex(0);
        MyMessageBox::ShowInformMessageBox("axis error, to small");
        return;
    } else if(axis.toInt() > RobotParams::dof){
        ui->comboBox_axisNum->setCurrentIndex(0);
        MyMessageBox::ShowInformMessageBox("axis error, to big");
        return;
    }

    m_paintGraph.PaintGraph(m_paintLogBuf, sectionIndex, name, axis);
}

void URI::on_pushButton_saveLogger_clicked()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    //UI请求日志后 控制器将把日志保存到ARM本地 请拷贝ARM上的日志文件
    //日志相对Modbus通讯来说太大, 后续可以考虑通过modbus进行分段传输

    m_unifiedRobot->SendMessageInformMsg(MsgStructure::MessageInform::SaveLogger);
}

void URI::on_pushButton_readLogger_clicked()
{
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    QString str = QFileDialog::getOpenFileName(Q_NULLPTR, "Choose logger file", conf->GetFilePath(Configuration::RootFolderPath).c_str());
    if(str.isEmpty()){
        return;
    }
    PRINTF(LOG_DEBUG, "%s: file=%s.\n", __func__, str.toStdString().c_str());

    int ret = NormalFile::ReadAllContents(str.toStdString().c_str(), m_paintLogBuf);
    std::string informStr;
    if(ret == 0){
        informStr = "日志读取完成";
    }else{
        informStr = "日志读取失败";
    }
    MyMessageBox::ShowInformMessageBox(informStr);
}

void URI::on_pushButton_enableLogger_clicked()
{
    bool isChecked = ui->pushButton_enableLogger->isChecked();

    double enableValue = isChecked ? 1.0 : 0.0;
    m_unifiedRobot->SendMessageInformMsg(MsgStructure::MessageInform::EnableLogger, enableValue);
    PRINTF(LOG_DEBUG, "%s: logger is %s.\n", __func__, isChecked? "opened": "closed");

    if(isChecked){//open
        ui->pushButton_enableLogger->setText(QObject::tr("关闭日志"));
    } else {//close
        ui->pushButton_enableLogger->setText(QObject::tr("启用日志"));
    }
}

void URI::CustomInitWithRobots()
{
    //初始化不同的机器人类型的特殊设置
    Init_MedicalUltrasoundRobot();
    Init_PedalRobot();
    Init_ScaraPaintRobot();
    Init_IgusFlexibleShaftRobot();
    Init_PedalShiftGearSwerveRobot();
}

/************************************超声波医疗4DOF机器人 DOF_4_MEDICAL************************************************/
void URI::EnableForceControl_MedicalUltrasoundRobot(bool flag)
{
    Q_UNUSED(flag);
#ifdef DOF_4_MEDICAL
    m_medicalUltrasoundRobot.EnableForceControl(flag);
#endif
}

void URI::Init_MedicalUltrasoundRobot()
{
}

void URI::UpdateLogic_MedicalUltrasoundRobot()
{
#ifdef DOF_4_MEDICAL
    //力传感器
    m_medicalUltrasoundRobot.CheckADForce();

    //按键
    int manualDir = m_medicalUltrasoundRobot.CheckADButton();
    if(manualDir != -1){//valid button
        manual_direction direction = (manual_direction)manualDir;
        int speedLevel = ui->verticalSlider_manualSpeed->value();
        int dofIndex = ManualDirection2DofIndex(direction);
        double manualSpeed = speedLevel * conf->manualSpeedRatio[dofIndex];
        m_unifiedRobot->SendManualControlMsg(mm_jog, direction, manualSpeed);
    }
#endif
}

void URI::UpdateInSwitchStatus_MedicalUltrasoundRobot()
{
#ifdef DOF_4_MEDICAL
    int inStatus = m_unifiedRobot->GetInSwitchStatus();
    m_medicalUltrasoundRobot.CheckServoAlarmSignal(inStatus);
#endif
}

/************************************油门刹车 dof_2_pedal************************************************/
void URI::Init_PedalRobot()
{
#ifdef ENABLE_PEDAL_ROBOT_WIDGETS
#ifdef NVH_USED
    //只保留设置tab
    ui->tabWidget1->removeTab(2);
    ui->tabWidget1->removeTab(1);
    ui->tabWidget1->removeTab(0);
#else
    if(conf->pedalRobotUsage != SettingWidgetPedalRobotGetSpeed::ManualEncoderControl){
        //只保留设置tab
        ui->tabWidget1->removeTab(2);
        ui->tabWidget1->removeTab(1);
        ui->tabWidget1->removeTab(0);
    }
#endif
#endif
}

/************************************Scara喷涂 dof_5_scara_paint************************************************/
void URI::Init_ScaraPaintRobot()
{
#ifdef DOF_5_SCARA_PAINT
    ui->label_theta1->setText("L1:");

    ui->pushButton_manualControlRXMinus->setText(tr("Rα-"));
    ui->pushButton_manualControlRXPlus->setText(tr("Rα+"));
    ui->pushButton_manualControlRYMinus->setText(tr("Rβ-"));
    ui->pushButton_manualControlRYPlus->setText(tr("Rβ+"));
#endif
}

/************************************Igus软轴 dof_5_igus_flexible_shaft************************************************/
void URI::Init_IgusFlexibleShaftRobot()
{
#ifdef DOF_5_IGUS_FLEXIBLE_SHAFT
    ui->pushButton_manualControlRXMinus->setText(tr("Rα-"));
    ui->pushButton_manualControlRXPlus->setText(tr("Rα+"));
    ui->pushButton_manualControlRYMinus->setText(tr("Rβ-"));
    ui->pushButton_manualControlRYPlus->setText(tr("Rβ+"));
#endif
}

void URI::Init_PedalShiftGearSwerveRobot()
{
#ifdef DOF_6_PEDAL_SHIFTGEAR_SWERVE
#endif
}
