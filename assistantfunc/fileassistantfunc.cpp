#include "fileassistantfunc.h"

#include <fstream>

#include "common/printf.h"

FileAssistantFunc::FileAssistantFunc()
{

}

bool FileAssistantFunc::IsFileExist(const std::string &filePath)
{
    std::fstream fs(filePath.c_str(), std::fstream::in | std::fstream::binary);
    if(fs.fail()){
        return false;
    }
    return true;
}

bool FileAssistantFunc::ReadFileContent(const std::string &filePath, std::string &fileContent)
{
    std::fstream fs(filePath.c_str(), std::fstream::in | std::fstream::binary);
    if(fs.fail()){
        PRINTF(LOG_ERR, "%s: open fstream fail(%s).\n", __func__, filePath.c_str());
        return false;
    }

    fileContent.clear();
    while(!fs.eof()){
        std::string content;
        fs >> content;

        fileContent += content + "\n";
    }

    fs.close();

    PRINTF(LOG_DEBUG, "%s: fileContent=%s\n", __func__, fileContent.c_str());

    return true;
}

bool FileAssistantFunc::WriteFileContent(const std::string &filePath, const std::string &fileContent)
{
    std::fstream fs(filePath.c_str(), std::fstream::trunc | std::fstream::out | std::fstream::binary);
    if(fs.fail()){
        PRINTF(LOG_ERR, "%s: open fstream fail(%s).\n", __func__, filePath.c_str());
        return false;
    }

    fs << fileContent;
    fs.close();

    return true;
}
