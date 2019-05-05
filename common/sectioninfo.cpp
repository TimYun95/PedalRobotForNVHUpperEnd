#include "sectioninfo.h"

#include <fstream>
#include <string.h>

#include "printf.h"
#include "assistantfunc/fileassistantfunc.h"

SectionInfoPtrList SectionInfo::ParseSectionInfoListFromFile(const std::string &filePath)
{
    std::string fileContent;
    bool readOk = FileAssistantFunc::ReadFileContent(filePath, fileContent);
    if(!readOk){
        PRINTF(LOG_ERR, "%s: read file content error...\n", __func__);
        return SectionInfoPtrList();
    }

    std::stringstream ss(fileContent);
    return ParseSectionInfoListFromStringStream(ss);
}

SectionInfoPtrList SectionInfo::ParseSectionInfoListFromStringStream(std::stringstream &ss)
{
    SectionInfoPtrList sectionList;

    std::string robotTypeStr;
    ss >> robotTypeStr;
    if(robotTypeStr.compare(RobotParams::robotType) != 0) {
        PRINTF(LOG_ERR, "%s: the teach file(%s) has wrong type of robot(%s)\n", __func__, robotTypeStr.c_str(), RobotParams::robotType);
        return sectionList;
    }

    sectionList.clear();
    while( true ){
        std::string modeStr;
        ss >> modeStr;
        if(modeStr.size() == 1 ){//section type
            PRINTF(LOG_DEBUG, "%s: section type=%s\n", __func__, modeStr.c_str());

            SectionInfoPtr sec;
            switch(modeStr[0]){
            case 'Q': sec.reset(new QSectionInfo());    break;
            case 'T': sec.reset(new TSectionInfo());    break;
            case 'L': sec.reset(new LSectionInfo());    break;
            case 'D': sec.reset(new DSectionInfo());    break;
            case 'S': sec.reset(new SSectionInfo());    break;
            case 'C': sec.reset(new CSectionInfo('C')); break;
            case 'O': sec.reset(new CSectionInfo('O')); break;
            case 'J': sec.reset(new JSectionInfo());    break;
            case 'M': sec.reset(new MSectionInfo());    break;
            case 'V': sec.reset(new VSectionInfo());    break;
            case 'R': sec.reset(new RSectionInfo());    break;
            default:
                PRINTF(LOG_ERR, "%s: unknown section\n",__func__);
                break;
            }
            if(sec){
                PRINTF(LOG_DEBUG, "%s: deserialize type=%c\n", __func__, sec->GetMode());
                sec->DeSerialize(ss);
                sectionList.push_back(sec);
            }
        }else if(modeStr.size() == 0){
            PRINTF(LOG_DEBUG, "%s: empty modeStr, read ok\n", __func__);
            break;
        }else{
            PRINTF(LOG_ERR, "%s: error modeStr=[%s]\n", __func__, modeStr.c_str());
            break;
        }
    }
    PRINTF(LOG_DEBUG, "%s: parse OK!\n",__func__);
    return sectionList;
}

bool SectionInfo::StringifySectionInfoListIntoFile(SectionInfoPtrList &sectionInfoList, const std::string &filePath)
{
    std::stringstream ss;
    bool stringifyOk = StringifySectionInfoListIntoStringStream(sectionInfoList, ss);
    if(!stringifyOk){
        PRINTF(LOG_ERR, "%s: error stringify\n", __func__);
        return false;
    }

    return FileAssistantFunc::WriteFileContent(filePath, ss.str());
}

bool SectionInfo::StringifySectionInfoListIntoStringStream(SectionInfoPtrList &sectionInfoList, std::stringstream &ss)
{
    ss << RobotParams::robotType << "\n";

    for(size_t i=0; i<sectionInfoList.size(); ++i){
        const SectionInfoPtr &sectionInfo = sectionInfoList[i];
        ss << sectionInfo->Serialize();
    }

    return true;
}
