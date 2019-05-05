#include "mymessagebox.h"

#include <QMessageBox>

void MyMessageBox::ShowInformMessageBox(const std::string &content)
{
    QString informContent = QObject::tr(content.c_str());
    QMessageBox::information(Q_NULLPTR, "Inform", informContent) ;
}

bool MyMessageBox::ShowQuestionMessageBox(const std::string &content)
{
    QString questionContent = QObject::tr(content.c_str());
    QMessageBox::StandardButton button = QMessageBox::question(Q_NULLPTR, "Question", questionContent) ;
    if(button == QMessageBox::StandardButton::Yes){
        return true;
    }else{
        return false;
    }
}

int MyMessageBox::ShowQueryMessageBox(const std::string &content, const std::string &buttonText0, const std::string &buttonText1, const std::string &buttonText2)
{
    QString queryContent = QObject::tr(content.c_str());
    int buttonIndex = QMessageBox::question(Q_NULLPTR, "Query", queryContent,
                                            QString::fromStdString(buttonText0),
                                            QString::fromStdString(buttonText1),
                                            QString::fromStdString(buttonText2));
    return buttonIndex;
}
