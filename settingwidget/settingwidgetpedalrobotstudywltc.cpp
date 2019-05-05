#include "settingwidgetpedalrobotstudywltc.h"
#include "ui_settingwidgetpedalrobotstudywltc.h"

#include <fstream>

#include <QFileDialog>
#include <QMessageBox>

#include "fileoperation/normalfile.h"

const std::string WltcPrefix("ParamsForWltc");

SettingWidgetPedalRobotStudyWltc::SettingWidgetPedalRobotStudyWltc(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetPedalRobotStudyWltc),
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

SettingWidgetPedalRobotStudyWltc::~SettingWidgetPedalRobotStudyWltc()
{
    delete ui;
}

void SettingWidgetPedalRobotStudyWltc::LoadParameters(Configuration &conf)
{
    for(int i=0; i<lineEditNum; ++i){
        SetLineEditValue(mylineEdit[i], conf.sysControlParamsWltc[i]);
    }

    myConf = &conf;
    UpdateCarTypeDisplay();
}

bool SettingWidgetPedalRobotStudyWltc::StoreParameters(Configuration &conf)
{
    for(int i=0; i<lineEditNum; ++i){
        conf.sysControlParamsWltc[i] = GetLineEditValue(mylineEdit[i]);
    }
    return true;
}

void SettingWidgetPedalRobotStudyWltc::on_pushButton_loadCarTypeConfFile_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(NULL, QObject::tr("加载WLTC车型文件"), parameterFilePath.c_str());
    if(filePath.isEmpty()){
        return;
    }
    myConf->defaultCarTypeFile = filePath.toStdString();

    ReadCarTypeConfFile(filePath.toStdString().c_str());
    UpdateCarTypeDisplay();
}

void SettingWidgetPedalRobotStudyWltc::on_pushButton_saveCarTypeConfFile_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(NULL, QObject::tr("保存WLTC车型文件"), parameterFilePath.c_str());
    if(filePath.isEmpty()){
        return;
    }
    myConf->defaultCarTypeFile = filePath.toStdString();

    StoreParameters(*myConf);
    SaveCarTypeConfFile(filePath.toStdString().c_str());
    QMessageBox::information(NULL, "inform", QObject::tr("WLTC车型文件保存成功!"));
    UpdateCarTypeDisplay();
}

void SettingWidgetPedalRobotStudyWltc::ReadCarTypeConfFile(const char *filePath)
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
    if(fileType != WltcPrefix){
        QMessageBox::warning(NULL, "Warning", QObject::tr("所选择的文件不是WLTC参数文件!"));
        ifs.close();
        return;
    }
#endif

    double value;
    for(size_t i=0; i<sizeof(myConf->sysControlParamsWltc)/sizeof(myConf->sysControlParamsWltc[0]); ++i){
        ifs>>value;
        SetLineEditValue(mylineEdit[i], value);
    }
    ifs.close();
}

void SettingWidgetPedalRobotStudyWltc::SaveCarTypeConfFile(const char *filePath)
{
    std::ofstream ofs(filePath, std::fstream::binary);
    if(ofs.fail()){
        PRINTF(LOG_ERR, "%s: fail to open file=%s\n", __func__, filePath);
        return;
    }

#ifdef ENABLE_CHECK_STUDY_FILE_TYPE
    //output type first
    ofs<<WltcPrefix<<"\n";
#endif

    for(size_t i=0; i<sizeof(myConf->sysControlParamsWltc)/sizeof(myConf->sysControlParamsWltc[0]); ++i){
        ofs<<myConf->sysControlParamsWltc[i]<<"\n";
    }
    ofs.close();
}

void SettingWidgetPedalRobotStudyWltc::UpdateCarTypeDisplay()
{
    const std::string carTypeText = NormalFile::GetFileName(myConf->defaultCarTypeFile.c_str());
    ui->label_carType->setText( tr("车型:") + QString::fromStdString(carTypeText));
}

void SettingWidgetPedalRobotStudyWltc::on_pushButton_readCalibratedParams_clicked()
{
    std::string filePath = "D:/WltcParams.txt";
    if(!QFile::exists(filePath.c_str())){
        QMessageBox::warning(Q_NULLPTR, "Warning", tr("无法打开WLTC调整参数文件!"));
        return;
    }

    ReadCarTypeConfFile(filePath.c_str());
    emit UpdateWltcParamsSignal();
}
