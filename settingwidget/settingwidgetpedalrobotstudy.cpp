#include "settingwidgetpedalrobotstudy.h"
#include "ui_settingwidgetpedalrobotstudy.h"

#include <fstream>

#include <QFileDialog>
#include <QMessageBox>

#include "fileoperation/normalfile.h"

const std::string NedcPrefix("ParamsForNedc");

SettingWidgetPedalRobotStudy::SettingWidgetPedalRobotStudy(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotStudy),
    parameterFilePath(Configuration::Instance()->GetFilePath(Configuration::RootFolderPath) + "/parameter_files")
{
    ui->setupUi(this);
    level=SettingBase::NormalUser;

    mylineEdit[0] = ui->lineEdit;
    mylineEdit[1] = ui->lineEdit_2;
    mylineEdit[2] = ui->lineEdit_3;
    mylineEdit[3] = ui->lineEdit_4;
    mylineEdit[4] = ui->lineEdit_5;
    mylineEdit[5] = ui->lineEdit_6;
    mylineEdit[6] = ui->lineEdit_7;
}

SettingWidgetPedalRobotStudy::~SettingWidgetPedalRobotStudy()
{
    delete ui;
}

void SettingWidgetPedalRobotStudy::LoadParameters(Configuration &conf)
{
    for(int i=0; i<lineEditNum; ++i){
       SetLineEditValue(mylineEdit[i], conf.sysControlParams[i]);
    }

    myConf = &conf;
    UpdateCarTypeDisplay();
}

bool SettingWidgetPedalRobotStudy::StoreParameters(Configuration &conf)
{
    if(CheckValid() == false){
        return false;
    }

    for(int i=0; i<lineEditNum; ++i){
        conf.sysControlParams[i] = GetLineEditValue(mylineEdit[i]);
    }
    return true;
}

bool SettingWidgetPedalRobotStudy::CheckValid()
{
#ifdef DISABLE_STUDY_LIMIT
    return true;
#else
    const double lowerBound[3] = {0.0, 20.0, 0.0};
    const double upperBound[3] = {0.2, 50.0, 1000.0};
    for(int i=0; i<3; ++i){
        double value = GetLineEditValue(mylineEdit[i]);
        if(value < lowerBound[i] || value > upperBound[i]){
            mylineEdit[i]->setText("请重新输入!");
            QMessageBox::warning(NULL, "warning", QObject::tr("数值设定越界!请重新设置!"));
            return false;
        }
    }

    return true;
#endif
}

void SettingWidgetPedalRobotStudy::ReadCarTypeConfFile(const char *filePath)
{
    std::ifstream ifs(filePath, std::fstream::binary);
    if(ifs.fail()){
        PRINTF(LOG_ERR, "%s: fail to open file=%s\n", __func__, filePath);
        return;
    }

#ifdef ENABLE_CHECK_STUDY_FILE_TYPE
    //type check
    std::string fileType;
    ifs>>fileType;
    if(fileType != NedcPrefix){
        QMessageBox::warning(NULL, "Warning", QObject::tr("所选择的文件不是NEDC参数文件!"));
        ifs.close();
        return;
    }
#endif

    double value;
    for(size_t i=0; i<sizeof(myConf->sysControlParams)/sizeof(myConf->sysControlParams[0]); ++i){
        ifs>>value;
        SetLineEditValue(mylineEdit[i], value);
    }
    ifs.close();
}

void SettingWidgetPedalRobotStudy::SaveCarTypeConfFile(const char *filePath)
{
    std::ofstream ofs(filePath, std::fstream::binary);
    if(ofs.fail()){
        PRINTF(LOG_ERR, "%s: fail to open file=%s\n", __func__, filePath);
        return;
    }

#ifdef ENABLE_CHECK_STUDY_FILE_TYPE
    //output type first
    ofs<<NedcPrefix<<"\n";
#endif

    for(size_t i=0; i<sizeof(myConf->sysControlParams)/sizeof(myConf->sysControlParams[0]); ++i){
        ofs<<myConf->sysControlParams[i]<<"\n";
    }
    ofs.close();
}

void SettingWidgetPedalRobotStudy::UpdateCarTypeDisplay()
{
    const std::string carTypeText = NormalFile::GetFileName(myConf->defaultCarTypeFile.c_str());
    ui->label_carType->setText( tr("车型:") + QString::fromStdString(carTypeText));
}

void SettingWidgetPedalRobotStudy::on_pushButton_loadCarTypeConfFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(NULL, QObject::tr("加载NEDC车型文件"), parameterFilePath.c_str());
    if(filePath.isEmpty()){
        return;
    }
    myConf->defaultCarTypeFile = filePath.toStdString();

    ReadCarTypeConfFile(filePath.toStdString().c_str());
    UpdateCarTypeDisplay();
}

void SettingWidgetPedalRobotStudy::on_pushButton_saveCarTypeConfFile_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(NULL, QObject::tr("保存NEDC车型文件"), parameterFilePath.c_str());
    if(filePath.isEmpty()){
        return;
    }
    myConf->defaultCarTypeFile = filePath.toStdString();

    StoreParameters(*myConf);
    SaveCarTypeConfFile(filePath.toStdString().c_str());
    QMessageBox::information(NULL, "inform", QObject::tr("NEDC车型文件保存成功!"));
    UpdateCarTypeDisplay();
}

void SettingWidgetPedalRobotStudy::on_pushButton_readCalibratedParams_clicked()
{
    std::string filePath = "D:/NedcParams.txt";
    if(!QFile::exists(filePath.c_str())){
        QMessageBox::warning(Q_NULLPTR, "Warning", tr("无法打开NEDC调整参数文件!"));
        return;
    }

    ReadCarTypeConfFile(filePath.c_str());
    emit UpdateNedcParamsSignal();
}
