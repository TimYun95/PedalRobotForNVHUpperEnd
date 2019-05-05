#include "myinputdialog.h"
#include "ui_myinputdialog.h"

#include <QDoubleValidator>

MyInputDialog::MyInputDialog(QWidget *parent,const QString &title, const QString &text,
                             QLineEdit::EchoMode mode, bool *ok)
    :QDialog(parent), ui(new Ui::MyInputDialog), _ok(ok)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->label->setText(text);

    ui->lineEdit->setEchoMode(mode);
}

MyInputDialog::MyInputDialog(QWidget *parent, const QString &title, const QString &text,
                             double min, double max, double showValue, bool *ok)
    :QDialog(parent), ui(new Ui::MyInputDialog), _ok(ok)
{
    ui->setupUi(this);
    this->setWindowTitle(title);
    ui->label->setText(text);

    QDoubleValidator v;
    v.setRange(min, max);
    ui->lineEdit->setValidator(&v);
    ui->lineEdit->setText(QString::number(showValue));
}

MyInputDialog::~MyInputDialog()
{
    delete ui;
}

QString MyInputDialog::GetText(QWidget *parent, const QString &title, const QString &label,
                               QLineEdit::EchoMode mode,bool *ok)
{
    MyInputDialog* dialog = new MyInputDialog(parent,title,label,mode,ok);
    dialog->open();
    while(!dialog->isHidden()){
        //Sleep(5000);
        QApplication::processEvents();
    }
    dialog->deleteLater();
    return dialog->str;
}

double MyInputDialog::GetNumber(QWidget *parent, const QString &title, const QString &label,
                             double min, double max, double showValue, bool *ok)
{
    MyInputDialog* dialog=new MyInputDialog(parent,title,label,min,max,showValue,ok);
    dialog->open();
    while(!dialog->isHidden()){
        //usleep(5000);
        QApplication::processEvents();
    }
    dialog->deleteLater();
    return dialog->value;
}

void MyInputDialog::on_pushButton_ok_clicked()
{
    str = ui->lineEdit->text();
    value = str.toDouble();
    *_ok = true;
    this->accept();
}

void MyInputDialog::on_pushButton_cancel_clicked()
{
    *_ok = false;
    this->reject();
}
