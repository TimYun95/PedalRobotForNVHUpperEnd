#include "settingwidgethardwareinterface.h"
#include "ui_settingwidgethardwareinterface.h"

#include <QMessageBox>
#include <QVector>

SettingWidgetHardwareInterface::SettingWidgetHardwareInterface(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetHardwareInterface)
{
    ui->setupUi(this);

    //该tab设置转接板上的接口设置（硬件接插件的接口）
    lineEdit_motorPort[5]=ui->lineEdit_motorPort6; lineEdit_encoderPort[5]=ui->lineEdit_encoderPort6; lineEdit_optSwitchPort[5]=ui->lineEdit_optSwitchPort6;
    lineEdit_motorPort[4]=ui->lineEdit_motorPort5; lineEdit_encoderPort[4]=ui->lineEdit_encoderPort5; lineEdit_optSwitchPort[4]=ui->lineEdit_optSwitchPort5;
    lineEdit_motorPort[3]=ui->lineEdit_motorPort4; lineEdit_encoderPort[3]=ui->lineEdit_encoderPort4; lineEdit_optSwitchPort[3]=ui->lineEdit_optSwitchPort4;
    lineEdit_motorPort[2]=ui->lineEdit_motorPort3; lineEdit_encoderPort[2]=ui->lineEdit_encoderPort3; lineEdit_optSwitchPort[2]=ui->lineEdit_optSwitchPort3;
    lineEdit_motorPort[1]=ui->lineEdit_motorPort2; lineEdit_encoderPort[1]=ui->lineEdit_encoderPort2; lineEdit_optSwitchPort[1]=ui->lineEdit_optSwitchPort2;
    lineEdit_motorPort[0]=ui->lineEdit_motorPort1; lineEdit_encoderPort[0]=ui->lineEdit_encoderPort1; lineEdit_optSwitchPort[0]=ui->lineEdit_optSwitchPort1;

    for(int i=RobotParams::axisNum; i<RobotParams::UIAxisNum; ++i){
        lineEdit_motorPort[i]->hide();
        lineEdit_encoderPort[i]->hide();
        lineEdit_optSwitchPort[i]->hide();
    }

    switch (RobotParams::axisNum){
    case 1:
        ui->label_2->hide();
        //no break!
    case 2:
        ui->label_3->hide();
        //no break!
    case 3:
        ui->label_4->hide();
        //no break!
    case 4:
        ui->label_5->hide();
        //no break!
    case 5:
        ui->label_6->hide();
    case 6:
        //do nothing
        break;
    default:
        PRINTF(LOG_CRIT, "%s: we have not considered this condition!\n", __func__);
        break;
    }
}

SettingWidgetHardwareInterface::~SettingWidgetHardwareInterface()
{
    delete ui;
}

//UI显示1~6 程序中保存、使用、传输的都是0~5
void SettingWidgetHardwareInterface::LoadParameters(Configuration &conf)
{
    for(int i=0;i<RobotParams::axisNum;i++){
        SetLineEditValue(lineEdit_motorPort[i], conf.motorPort[i]+1);
        SetLineEditValue(lineEdit_encoderPort[i], conf.encoderPort[i]+1);
        SetLineEditValue(lineEdit_optSwitchPort[i], conf.optSwitchPort[i]+1);
    }
}

bool SettingWidgetHardwareInterface::StoreParameters(Configuration &conf)
{
    if( !CheckValidityOfParameters() ){
        QMessageBox::information( NULL, "Warning", QObject::tr("接口页面设定参数无效!\n该页面参数将不被保存!")) ;
        LoadParameters(conf);
        return false;
    }

    for(int i=0;i<RobotParams::axisNum;i++){
        conf.motorPort[i]=GetLineEditValue(lineEdit_motorPort[i])-1;
        conf.encoderPort[i]=GetLineEditValue(lineEdit_encoderPort[i])-1;
        conf.optSwitchPort[i]=GetLineEditValue(lineEdit_optSwitchPort[i])-1;
    }
    return true;
}

bool SettingWidgetHardwareInterface::CheckValidityOfParameters()//UI显示1~6 进行检查:每一列的参数 1~6应当各出现一次
{
    int n;
    QVector<int> vecOfMotorPort(6,0), vecOfEncoderPort(6,0), vecOfOptSwitchPort(6,0);//出现次数
    for(int i=0; i<RobotParams::axisNum; i++){
        n = GetLineEditValue(lineEdit_motorPort[i]);
        if(n>0 && n<7){
            vecOfMotorPort[n-1]++;
        }else{
            return false;
        }

        n = GetLineEditValue(lineEdit_encoderPort[i]);
        if(n>0 && n<7){
            vecOfEncoderPort[n-1]++;
        }else{
            return false;
        }

        n = GetLineEditValue(lineEdit_optSwitchPort[i]);
        if(n>0 && n<7){
            vecOfOptSwitchPort[n-1]++;
        }else{
            return false;
        }
    }

    for(int i=0; i<RobotParams::axisNum; i++){
        if(vecOfMotorPort[i]>1 || vecOfEncoderPort[i]>1 || vecOfOptSwitchPort[i]>1){
            return false;
        }
    }

    return true;
}
