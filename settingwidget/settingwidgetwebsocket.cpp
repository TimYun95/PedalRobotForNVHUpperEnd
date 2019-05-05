#include "settingwidgetwebsocket.h"
#include "ui_settingwidgetwebsocket.h"

#include <QFileDialog>
#include <QMessageBox>

SettingWidgetWebSocket::SettingWidgetWebSocket(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingWidgetWebSocket)
{
    ui->setupUi(this);
}

SettingWidgetWebSocket::~SettingWidgetWebSocket()
{
    delete ui;
}

void SettingWidgetWebSocket::LoadParameters(Configuration &conf)
{
    ui->lineEdit_serverIP->setText( conf.wsServerIP.c_str() );
    ui->lineEdit_groupId->setText( conf.wsGroupId.c_str() );
    ui->lineEdit_groupPassword->setText( conf.wsPassword.c_str() );
    ui->lineEdit_rsaPublicKeyFilePath->setText( conf.wsRsaPublicKeyFilePath.c_str() );
    ui->comboBox_communicationRole->setCurrentIndex( conf.wsCmtRole );
}

bool SettingWidgetWebSocket::StoreParameters(Configuration &conf)
{
    conf.wsServerIP = ui->lineEdit_serverIP->text().toStdString();

    std::string str = ui->lineEdit_groupId->text().toStdString();
    if(CheckStringLen(str)){
        conf.wsGroupId = str;
    }else{
        return DisplayErrorWarningBox(str);
    }

    str = ui->lineEdit_groupPassword->text().toStdString();
    if(CheckStringLen(str)){
        conf.wsPassword = str;
    }else{
        return DisplayErrorWarningBox(str);
    }

    conf.wsRsaPublicKeyFilePath = ui->lineEdit_rsaPublicKeyFilePath->text().toStdString();
    conf.wsCmtRole = ui->comboBox_communicationRole->currentIndex();

    return true;
}

bool SettingWidgetWebSocket::CheckStringLen(const std::string &str)
{
    //32B长度限制 原因见websocketmessage.cpp的segmentLen
    if(str.length() > 32){
        PRINTF(LOG_ERR, "%s: error length of %s\n", __func__, str.c_str());
        return false;
    }
    return true;
}

bool SettingWidgetWebSocket::DisplayErrorWarningBox(const std::string &str)
{
    QMessageBox::warning(NULL, "Warning", QObject::tr("SettingWidgetWebSocket:\n字符长度太长:") + QString(str.c_str()));
    return false;
}

void SettingWidgetWebSocket::on_pushButton_browse_clicked()
{
    QString filePath = QFileDialog::getOpenFileName(NULL, tr("请选择WebSocket加密通讯RSA密钥"));
    if(filePath.isEmpty()){
        return;
    }

    if( !filePath.endsWith(".key") ){
        QMessageBox::warning(NULL, "Warning", tr("无效的RSA密钥(应为.key文件)!"));
        return;
    }

    ui->lineEdit_rsaPublicKeyFilePath->setText( filePath );
}
