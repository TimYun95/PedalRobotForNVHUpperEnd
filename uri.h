#ifndef URI_H
#define URI_H

#include <map>

#include <QString>
#include <QWidget>
#include <QSlider>
#include <QPushButton>

#include "teach.h"
#include "configuration.h"
#include "common/message.h"
#include "unifiedrobot.h"
#include "mywidget/my2dgraph.h"
#include "util/highprecisiontimer.h"

#ifdef DOF_4_MEDICAL
 #include "robottype/medicalultrasoundrobot/medicalultrasoundrobot.h"
#endif

class SettingBase;
class QListWidgetItem;
class CommunicationMediator;

namespace Ui {
class URI;
}

class URI : public QWidget
{
    Q_OBJECT
public:
    explicit URI(QWidget *parent, bool enableLogicUpdateTimer, bool enableWidgetUpdateTimer);
    ~URI();

    void UpdateLogic();
    void UpdateWidgets();

private:
    void InitSettingsWidgets();
    void InitSettingsWidgetWithRobots();
    void DisplaySettingWidget(SettingBase* settingBase, bool showFlag);
    void ShowSettingWidgets(int settingLevel);
    void HideSettingWidgets();

    void DisplayWidget_AtSuspendPoint();
    void DisplayWidget_FinishGoHome(int goHomeResult);

    void EnableGoHomeWidgets(bool enableFlag);

    void ResetSingleAxisWidgets();
    void HideSingleAxisWidget(int startAxis);
    int SingleAxisSlider2AxisIndex(QSlider* singleAxisSlider);
    double SingleAxisSlider2Speed(QSlider* singleAxisSlider);
    int SingleAxisButton2AxisIndex(QPushButton* btn);
    int SingleAxisButton2Direction(QPushButton* btn);

    manual_direction ManualControlButton2Direction(QPushButton* btn);
    manual_method ManualMethodComboBox2Method();
    int ManualDirection2DofIndex(manual_direction direction);
    double ManualSpeedSlider2Speed(manual_direction direction);

    void ResetActionWidget();

    void DisableTeachWidgets();
    void ResetTeachWidgetVibration();
    void UpdateTeachWidgetSectionList();

private:
    void InitSignalSlot();
    void InitWidgetDisplay();
    void InitSingleAxisWidgets();
    void InitManualControlButtonWidgets();

private:
    void UpdateWidgetByThetaMatrix();
    void UpdateWidgetByActionMsg();
    void UpdateWidgetByCmtConnection();

signals:
    void Signal_MedicalUltrasoundRobotAxis4Theta();
    void Signal_RobotEmergencyStop(bool stopSuccess);
    void Signal_RobotGoHomeResult(bool result);

private:
    void RegisterUpdateActionCallback();
    void On_UpdateGoHomeResult();
    void On_UpdateStopWeld();
    void On_UpdateOutControlStatus();
    void On_UpdateInSwitchStatus();
    void On_UpdateMessageInform();
    void On_UpdateUnifiedInform();
    void on_UpdateReInit();
    void On_UpdateFinishAction();
    void On_UpdateAtSuspendPoint();

public slots:
    void ss_on_logicUpdateTimer_timeout();
    void ss_on_widgetUpdateTimer_timeout();

public slots:
    /*手动-当前位置*/
    void on_pushButton_origin_clicked();
    void on_pushButton_softstop_clicked();

    /*手动-手动调节*/
    void ss_on_pushButton_manualControlN_pressed();
    void ss_on_pushButton_manualControlN_released();
    void ss_on_pushButton_manualControlN_clicked();

    /*手动-单轴控制*/
    void ss_on_horizontalSlider_singleAxisN_valueChanged(int value);
    void ss_on_pushButton_stopAxisN_pressed();
    void ss_on_pushButton_stopAxisN_released();
    void ss_on_pushButton_stopAxisN_clicked();

    /*手动-输出控制*/
    void ss_on_pushButton_outCtrlN_clicked();

    /*执行*/
    void on_pushButton_chooseFile_clicked();//选择文件
    void on_pushButton_startArc_clicked();//开始执行
    void on_pushButton_stopWeld_clicked();//结束执行
    void on_pushButton_chooseSection_clicked();//选段执行
    void on_pushButton_continous_clicked();//连续执行
    void on_pushButton_startToWork_clicked();//开始工作
    void on_pushButton_pause_clicked();//暂停

    /*示教*/
    void on_pushButton_startTeach_clicked();
    void on_pushButton_stopTeach_clicked();
    void on_pushButton_trans_clicked();
    void on_pushButton_suspend_clicked();
    void on_pushButton_joint_clicked();
    void on_pushButton_plc_clicked();

    void on_pushButton_delete_clicked();
    void on_pushButton_addSection_clicked();
    void on_pushButton_attribute_clicked();
    void on_pushButton_undo_clicked();
    void on_pushButton_redo_clicked();

    void on_pushButton_line1_clicked();
    void on_pushButton_line2_clicked();
    void on_pushButton_clear1_clicked();

    void on_pushButton_arc1_clicked();
    void on_pushButton_arc2_clicked();
    void on_pushButton_arc3_clicked();
    void on_pushButton_clear2_clicked();

    void on_pushButton_conFirst_clicked();
    void on_pushButton_conNext_clicked();
    void on_pushButton_conFinish_clicked();
    void on_pushButton_clear3_clicked();

    void on_pushButton_dot_clicked();

    void on_pushButton_editTeach_clicked();
    
    /*设置*/
    void on_updateSettingWidgets(QListWidgetItem *current, QListWidgetItem *previous);
    void on_pushButton_changePassword_clicked();
    void on_pushButton_saveSettings_clicked();
    void on_pushButton_exitSettings_clicked();

    /*日志*/
    void ss_on_comboBox_currentIndexChanged(QString text);
    void on_pushButton_saveLogger_clicked();
    void on_pushButton_readLogger_clicked();
    void on_pushButton_enableLogger_clicked();

private:
    Ui::URI *ui;

    UnifiedRobot* m_unifiedRobot;
    Teach* mTeach;
    Configuration* conf;
    RobotThetaMatrix* rbtThetaMatrix;

    std::map<QListWidgetItem* , SettingBase*> settingMap;
    HighPrecisionTimer m_logicUpdateTimer;
    HighPrecisionTimer m_widgetUpdateTimer;
    int m_settingLevel;

    My2DGraph m_paintGraph;
    QString m_paintLogBuf;

private:
    void CustomInitWithRobots();
/******************************超声波医疗 DOF_4_MEDICAL*******************************/
public:
    void EnableForceControl_MedicalUltrasoundRobot(bool flag);
private:
    void Init_MedicalUltrasoundRobot();
    void UpdateLogic_MedicalUltrasoundRobot();
    void UpdateInSwitchStatus_MedicalUltrasoundRobot();
#ifdef DOF_4_MEDICAL
private:
    MedicalUltrasoundRobot m_medicalUltrasoundRobot;
#endif

/******************************油门刹车踏板 DOF_2_PEDAL 均以pd开头*******************************/
private:
    void Init_PedalRobot();

/******************************5轴Scara喷涂机器人******************************/
private:
    void Init_ScaraPaintRobot();

/******************************5轴Igus软轴机器人******************************/
private:
    void Init_IgusFlexibleShaftRobot();

/******************************6轴踏板换挡换向机器人******************************/
private:
    void Init_PedalShiftGearSwerveRobot();

/******************************6轴踏板换挡换向机器人******************************/
signals:
    void Signal_WidgetShowInformMessageBox(QString str);

/******************************2轴踏板NVH机器人******************************/
#ifdef NVH_USED
private:
    const std::string rootPwd;
#endif
};

#endif // URI_H
