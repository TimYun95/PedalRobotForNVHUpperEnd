#ifndef MYWIDGETHELPER_H
#define MYWIDGETHELPER_H

#include <QPushButton>
#include <QLineEdit>

class MyWidgetHelper
{
public:
    static void SetOutCtrlButtonStatus(QPushButton* button, bool onOffStatus);
    static void SetOutCtrlLineEditStatus(QLineEdit* lineEdit, bool onOffStatus);

    static void SetInSwitchLineEditStatus(QLineEdit* lineEdit, bool onOffStatus);

    static void SetLineEditValue(QLineEdit* lineEdit, int value);
    static void SetLineEditValue(QLineEdit* lineEdit, double value, char format='f', int prec=3);
};

#endif // MYWIDGETHELPER_H
