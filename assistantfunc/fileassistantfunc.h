#ifndef FILEASSISTANTFUNC_H
#define FILEASSISTANTFUNC_H

#include <string>

class FileAssistantFunc
{
public:
    FileAssistantFunc();

    static bool IsFileExist(const std::string &filePath);
    static bool ReadFileContent(const std::string &filePath, std::string &fileContent);
    static bool WriteFileContent(const std::string &filePath, const std::string &fileContent);
};

#endif // FILEASSISTANTFUNC_H
