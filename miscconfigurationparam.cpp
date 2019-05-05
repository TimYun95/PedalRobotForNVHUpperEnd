#include "miscconfigurationparam.h"

#include "common/printf.h"

MiscConfigurationParam::MiscConfigurationParam()
{
}

MiscConfigurationParam::~MiscConfigurationParam()
{
}

void MiscConfigurationParam::SetConfigParam(MiscConfigurationParam::eMiscConfType miscConfType, int configParam)
{
    m_confType2Param[miscConfType] = configParam;
}

int MiscConfigurationParam::GetConfigParam(MiscConfigurationParam::eMiscConfType miscConfType)
{
    std::map<eMiscConfType, int>::iterator itr = m_confType2Param.find(miscConfType);
    if(itr == m_confType2Param.end()){
        PRINTF(LOG_ERR, "%s: invalid miscConfType!\n", __func__);
        return 0;
    }

    return itr->second;
}
