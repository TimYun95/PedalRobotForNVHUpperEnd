#include "sectionattributedialog.h"
#include "ui_sectionattributedialog.h"

SectionAttributeDialog::SectionAttributeDialog(char mod,double* transSpeed,int* dotTime,double* weldSpeed,
                                               int* vibType,double* vibrationFreqFB,double* vibrationAmplFB,
                                               double* vibrationFreqLR,double* vibrationAmplLR,QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SectionAttributeDialog)
{
    ui->setupUi(this);
    _mod=mod;
    _transSpeed=transSpeed;//指针
    _dotTime=dotTime;
    _weldSpeed=weldSpeed;
    _vibType=vibType;
    _vibrationFreqFB=vibrationFreqFB;
    _vibrationAmplFB=vibrationAmplFB;
    _vibrationFreqLR=vibrationFreqLR;
    _vibrationAmplLR=vibrationAmplLR;

    ui->lineEdit_transSpeed      ->setText( QString::number( (_transSpeed ? *_transSpeed : 0) ) );
    ui->lineEdit_dotTime         ->setText( QString::number( (_dotTime ? *_dotTime : 0) ) );
    ui->lineEdit_weldSpeed       ->setText( QString::number( (_weldSpeed ? *_weldSpeed : 0) ) );
    ui->comboBox_vibrationType   ->setCurrentIndex( (_vibType ? *_vibType : 0) );
    ui->lineEdit_vibrationFreqFB ->setText( QString::number( (_vibrationFreqFB ? *_vibrationFreqFB : 0) ) );
    ui->lineEdit_vibrationAmplFB ->setText( QString::number( (_vibrationAmplFB ? *_vibrationAmplFB : 0) ) );
    ui->lineEdit_vibrationFreqLR ->setText( QString::number( (_vibrationFreqLR ? *_vibrationFreqLR : 0) ) );
    ui->lineEdit_vibrationAmplLR ->setText( QString::number( (_vibrationAmplLR ? *_vibrationAmplLR : 0) ) );

    switch(mod){
    case 'T':
        ui->widget_dot->hide();
        ui->widget_others->hide();
        break;
    case 'D':
        ui->widget_others->hide();
        break;
    case 'L':
    case 'C':
    case 'O':
    case 'S':
        break;
    }
}

SectionAttributeDialog::~SectionAttributeDialog()
{
    delete ui;
}

void SectionAttributeDialog::on_buttonBox_accepted()
{
    switch(_mod){
    case 'L':
    case 'C':
    case 'O':
    case 'S':
        *_transSpeed=ui->lineEdit_transSpeed->text().toDouble();//指针
        *_weldSpeed=ui->lineEdit_weldSpeed->text().toDouble();
        *_vibType=ui->comboBox_vibrationType->currentIndex();
        *_vibrationFreqFB=ui->lineEdit_vibrationFreqFB->text().toDouble();
        *_vibrationFreqLR=ui->lineEdit_vibrationFreqLR->text().toDouble();
        *_vibrationAmplFB=ui->lineEdit_vibrationAmplFB->text().toDouble();
        *_vibrationAmplLR=ui->lineEdit_vibrationAmplLR->text().toDouble();
        break;
    case 'D':
        *_transSpeed=ui->lineEdit_transSpeed->text().toDouble();
        *_dotTime=ui->lineEdit_dotTime->text().toInt();
        break;
    case 'T':
        *_transSpeed=ui->lineEdit_transSpeed->text().toDouble();
        break;
    }
    this->destroy();
}

void SectionAttributeDialog::on_buttonBox_rejected()
{
    this->destroy();
}
