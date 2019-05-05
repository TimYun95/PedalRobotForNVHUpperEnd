#ifndef MISCCONFIGURATIONPARAM_H
#define MISCCONFIGURATIONPARAM_H

#include <map>
#include "template/singleton.h"

class MiscConfigurationParam : public Singleton<MiscConfigurationParam>
{
public:
    MiscConfigurationParam();
    virtual ~MiscConfigurationParam();

    enum eMiscConfType{
        eEnableShowBoxWhenFinishGoHome,
        eEnableShowBoxWhenReceiveMessageInform,
        eEnablePedalSystemControl,

        MaxMiscConfType
    };

    void SetConfigParam(eMiscConfType miscConfType, int configParam);
    int GetConfigParam(eMiscConfType miscConfType);

private:
    std::map<eMiscConfType, int> m_confType2Param;
};

#endif // MISCCONFIGURATIONPARAM_H
