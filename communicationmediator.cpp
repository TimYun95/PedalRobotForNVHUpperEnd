#include "communicationmediator.h"

#include <QString>

#include "common/printf.h"
#include "configuration.h"

int CommunicationMediator::role = CommunicationMediator::UnvalidClient;

Message CommunicationMediator::CheckMsg(const ReceiveMsgType type)
{
    if(type == ModbusMsg){//检查Modbus消息
        if(role == LocalClient){//只有本地UI 才需要Modbus
            assert(modbusCmt);
            return modbusCmt->CheckMsg();
        }else{
            return Message();//isNull=true
        }
    }else{//检查WebSocket消息
        int ret = wsAdapter->CheckMsg();
        if(ret == -1){
            return Message();//isNull=true
        }else{
            return Message(ret, 0, 0);//isNull=false
        }
    }
}

CommunicationMediator::CommunicationMediator()
    : modbusCmt(nullptr), wsAdapter(nullptr)
{
    Configuration* conf = Configuration::Instance();
    int cmtRole = conf->wsCmtRole;
    switch(cmtRole){
    case 0://本地UI客户端
        role = LocalClient;
        break;
    case 1://远程UI客户端
        role = RemoteClient;
        break;
    default:
        PRINTF(LOG_ERR, "%s: unvalid role(%d)\n", __func__, cmtRole);
        break;
    }
    assert(role != UnvalidClient);//必须确保role有效

    if(role == LocalClient){//本地UI 才需要Modbus通讯
        if(modbusCmt == nullptr){
#ifdef WIN32
            const int uartComNum = conf->serialDevice + 1;//index=0 COM1
            const std::string comNum = std::string("COM") + QString::number(uartComNum).toStdString();
#else
            const int uartComNum = conf->serialDevice;//index=0 /dev/ttyUSB0
            const std::string comNum = std::string("/dev/ttyUSB") + QString::number(uartComNum).toStdString();
#endif
            modbusCmt = new Communicator(Communicator::UI, comNum.c_str());
        }
    }else{
        PRINTF(LOG_DEBUG, "%s: RemoteClient is no needed to create modbus communicator.\n", __func__);
    }

    if(wsAdapter == nullptr){
#ifdef ENABLE_WEBSOCKET_COMMUNICATION
        wsAdapter = new WebSocketAdapter(conf->wsGroupId, conf->wsPassword, conf->wsRsaPublicKeyFilePath);
#else
        wsAdapter = new WebSocketFakeAdapter(conf->wsGroupId, conf->wsPassword, conf->wsRsaPublicKeyFilePath);
#endif

        wsAdapter->Start(conf->wsServerIP, 9010);
    }
}

CommunicationMediator::~CommunicationMediator()
{
    if(modbusCmt){
        delete modbusCmt;
        modbusCmt = nullptr;
    }

    if(wsAdapter){
        wsAdapter->Stop();
        delete wsAdapter;
        wsAdapter = nullptr;
    }
}
