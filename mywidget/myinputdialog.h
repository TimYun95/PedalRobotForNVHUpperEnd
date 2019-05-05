#ifndef MYINPUTDIALOG_H
#define MYINPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>

namespace Ui {
class MyInputDialog;
}

class MyInputDialog : public QDialog
{
    Q_OBJECT
    
public:
    MyInputDialog(QWidget *parent, const QString &title, const QString &label,
                  QLineEdit::EchoMode mode, bool *ok);
    MyInputDialog(QWidget *parent, const QString &title, const QString &label,
                  double min, double max, double showValue, bool *ok);
    ~MyInputDialog();

    static QString GetText(QWidget *parent, const QString &title, const QString &label,
                           QLineEdit::EchoMode mode, bool *ok);
    static double GetNumber(QWidget *parent, const QString &title, const QString &label,
                      double min, double max, double showValue, bool *ok);

private slots:
    void on_pushButton_ok_clicked();
    void on_pushButton_cancel_clicked();

private:
    Ui::MyInputDialog *ui;
    QString str;
    double value;
    bool *_ok;
};

#endif // MYINPUTDIALOG_H
