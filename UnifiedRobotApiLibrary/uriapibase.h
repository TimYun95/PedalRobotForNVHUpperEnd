#ifndef URIAPIBASE_H
#define URIAPIBASE_H

#include <map>
#include <vector>
#include <string>
#include <tr1/functional>

#include "apidllexportdefine.h"

class URI;
class Configuration;

class UNIFIED_ROBOT_DLL_EXPORT UriApiBase
{
public:
    UriApiBase(bool enableLogicUpdateTimer, bool enableWidgetUpdateTimer);
    virtual ~UriApiBase();

    enum eApiCallbackType{
        eReceiveEmergencyStopResult,
        eReceiveUnifiedInform,

        eMaxApiCallbackType
    };
    typedef std::tr1::function<void ()> ApiCallbackFunc_t;

    int Api_ShowWidget(bool showFlag);
    int Api_GoHome(bool popupConfirmBox);
    int Api_StopSingleAxis(const std::vector<int> &stopAxes);
    int Api_MoveSingleAxis(const std::vector<int> &moveAxes, const std::vector<double> &moveSpeed);
    int Api_SwitchToAction(const std::string &actionFileContent);
    int Api_SetMonitorActionTheta(const std::vector<int> &actionMethod, const std::vector<int> &actionAxes, const std::vector<double> &actionTheta, int customVariable);
    int Api_SwitchToIdleState();
    int Api_SetVelocityActionSpeed(const std::vector<double> &actionSpeed);
    int Api_SetSerialPort(int serialDeviceIndex);
    int Api_SetPositionLimitConf(const std::vector<double> &positiveLimit, const std::vector<double> &negativeLimit);
    int Api_SetReservedParamConf(const std::vector<double> &reservedParam);
    int Api_SaveAndSendConfMsg(bool saveFlag, bool sendFlag);
    int Api_MessageInformMsg(int informType, double informValue);

    bool Api_GetGoHomeStatus();
    bool Api_GetIsEmergencyStopSuccess();
    std::vector<double> Api_GetRobotTheta();
    std::vector<double> Api_GetRobotMatrix();
    void Api_GetUnifiedInform(std::vector<int>& intArray, std::vector<double>& doubleArray);
    std::string Api_GetStatusString();
    int Api_GetStatusStringIndex();
    std::vector<double> Api_GetPositionLimitConf(bool isPositive);

    bool Api_InstallCallback(eApiCallbackType apiCallbackType, ApiCallbackFunc_t apiCallbackFunc);
    void Api_InstallCallback_OnReceiveEmergencyStopSignal(std::tr1::function<void (bool)> &callbackFunc);
    void Api_InstallCallback_OnReceiveUnifiedInformSignal(std::tr1::function<void (const std::vector<int> &, const std::vector<double> &)> &callbackFunc);

#ifdef ENABLE_UNIFIED_ROBOT_EXPORT_API_DLL
private:
    void On_ReceiveMessageInform();
    void On_ReceiveUnifiedInform();

private:
    void InitUriApiBase();
    void DispatchCallback(eApiCallbackType apiCallbackType);

private:
    URI* m_uri;
    Configuration* m_conf;

    std::map<eApiCallbackType, ApiCallbackFunc_t> m_apiCallbackType2Func;
#endif
};

#endif // URIAPIBASE_H
