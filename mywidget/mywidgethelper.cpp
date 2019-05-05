#include "mywidgethelper.h"


void MyWidgetHelper::SetOutCtrlButtonStatus(QPushButton *button, bool onOffStatus)
{
    if(onOffStatus){
        button->setText(QObject::tr("关闭"));
    }else{
        button->setText(QObject::tr("打开"));
    }
}

void MyWidgetHelper::SetOutCtrlLineEditStatus(QLineEdit *lineEdit, bool onOffStatus)
{
    QPalette palette = lineEdit->palette();
    if(onOffStatus){
        palette.setColor(QPalette::Base,Qt::green);
        lineEdit->setPalette(palette);
    }else{
        palette.setColor(QPalette::Base,Qt::red);
        lineEdit->setPalette(palette);
    }
}

void MyWidgetHelper::SetInSwitchLineEditStatus(QLineEdit *lineEdit, bool onOffStatus)
{
    QPalette palette = lineEdit->palette();
    if(onOffStatus){
        palette.setColor(QPalette::Base,Qt::green);
        lineEdit->setPalette(palette);
    }else{
        palette.setColor(QPalette::Base,Qt::red);
        lineEdit->setPalette(palette);
    }
}

void MyWidgetHelper::SetLineEditValue(QLineEdit *lineEdit, int value)
{
    QString valueString = QString::number(value);
    lineEdit->setText(valueString);
}

void MyWidgetHelper::SetLineEditValue(QLineEdit *lineEdit, double value, char format, int prec)
{
    QString valueString = QString::number(value, format, prec);
    lineEdit->setText(valueString);
}
