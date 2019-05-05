#ifndef SECTIONATTRIBUTEDIALOG_H
#define SECTIONATTRIBUTEDIALOG_H

#include <QDialog>

namespace Ui {
class SectionAttributeDialog;
}

class SectionAttributeDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit SectionAttributeDialog(char mod,double* transSpeed=0,int* dotTime=0,double* weldSpeed=0,
                                    int* vibType=0,double* vibrationFreqFB=0,double* vibrationAmplFB=0,
                                    double* vibrationFreqLR=0,double* vibrationAmplLR=0,QWidget *parent = 0);//带默认形参的构造函数
    ~SectionAttributeDialog();
    
private slots:
    void on_buttonBox_accepted();

    void on_buttonBox_rejected();

private:
    Ui::SectionAttributeDialog *ui;

    char    _mod;
    double* _transSpeed;
    int*    _dotTime;
    double* _weldSpeed;
    int*    _vibType;
    double* _vibrationFreqFB;
    double* _vibrationAmplFB;
    double* _vibrationFreqLR;
    double* _vibrationAmplLR;
};

#endif // SECTIONATTRIBUTEDIALOG_H
