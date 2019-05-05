#include "settingwidgetforcecontrol.h"
#include "ui_settingwidgetforcecontrol.h"

SettingWidgetForceControl::SettingWidgetForceControl(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetForceControl)
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;
}

SettingWidgetForceControl::~SettingWidgetForceControl()
{
    delete ui;
}

void SettingWidgetForceControl::LoadParameters(Configuration &conf)
{
    SetLineEditValue(ui->lineEdit_forceThresholdValue, conf.forceValueThres);
    SetLineEditValue(ui->lineEdit_forceBalanceValue, conf.balanceForce);
}

bool SettingWidgetForceControl::StoreParameters(Configuration &conf)
{
    conf.forceValueThres=GetLineEditValue(ui->lineEdit_forceThresholdValue);
    conf.balanceForce=GetLineEditValue(ui->lineEdit_forceBalanceValue);
    return true;
}

void SettingWidgetForceControl::on_pushButton_forceControl_clicked()
{
    static bool checked=false;
    checked ^= true;

    emit ForceControlClickedSignal(checked);
    if(checked){
        ui->label_forceControlStatus->setText(QObject::tr("当前力控已开启..."));
        ui->pushButton_forceControl->setText(QObject::tr("关闭力控制"));
    }else{
        ui->label_forceControlStatus->setText(QObject::tr("当前力控已关闭..."));
        ui->pushButton_forceControl->setText(QObject::tr("开启力控制"));
    }
}

void SettingWidgetForceControl::ForceValueSlot(QString status)
{
    ui->lineEdit_forceValue->setText(status);
}
