#ifndef MYMESSAGEBOX_H
#define MYMESSAGEBOX_H

#include <string>

class MyMessageBox
{
public:
    static void ShowInformMessageBox(const std::string& content);
    static bool ShowQuestionMessageBox(const std::string& content);
    static int ShowQueryMessageBox(const std::string& content,
                                   const std::string& buttonText0 = std::string(),
                                   const std::string& buttonText1 = std::string(),
                                   const std::string& buttonText2 = std::string());
};

#endif // MYMESSAGEBOX_H
