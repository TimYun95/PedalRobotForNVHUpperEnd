#ifndef COMMUNICATIONMEDIATOR_H
#define COMMUNICATIONMEDIATOR_H

/* Mediator中介者模式
 * URI通过该类和Modbus+WebSocket进行通讯交互
 *
 * 通讯模型如下:
 *            Controller <----------> 本地UI <----------> 远程UI
 *   协议:                   Modbus            WebSocket
 *   角色:      Slave                 Master           /(无Modbus)
 *   角色: /(无WebSocket)          LocalClient         RemoteClient
 *
 * 通讯的处理:
 *  1) Controller:(运行于ARM上 与本部分程序无关)
 *    Controller<---本地UI 根据接收到的Modbus消息进行机器人的动作控制
 *    Controller--->本地UI 通过Modbus发送当前机器人状态至本地UI
 *  2) 本地UI: LocalClient
 *    Controller<---本地UI UI操作通过Modbus发送给Controller
 *    Controller--->本地UI 接收到的Modbus消息显示于本地UI界面
 *      本地UI  --->远程UI 接收到的Modbus消息通过WebSocket转发给远程UI
 *      本地UI  <---远程UI 接收到的WebSocket消息通过Modbus转发给Controller
 *  3) 远程UI: RemoteClient
 *      本地UI  --->远程UI 接收到的WebSocket消息显示于远程UI界面
 *      本地UI  <---远程UI UI操作通过WebSocket发送给本地UI
 *
 * 程序根据 通讯的角色(role) 以及 消息的走向(CommunicationDirection) 进行消息的分发
 */

#include <cassert>

#include "template/singleton.h"
#include "communication/msgstructure.h"
#include "communication/libmodbus/communicator.h"

#ifdef ENABLE_WEBSOCKET_COMMUNICATION
    #include "MyWebSocket/websocketadapter.h"
#else
    #include "MyWebSocket/websocketfakeadapter.h"
#endif

class CommunicationMediator : public Singleton<CommunicationMediator>
{
public:
    enum tagRole{
        UnvalidClient,
        LocalClient,//本地UI客户端
        RemoteClient//远程UI客户端
    };

    enum CommunicationDirection{
        UplinkMsg,//上行消息
        DownlinkMsg//下行消息
    };

    enum ReceiveMsgType{
        ModbusMsg,//Modbus的消息
        WebSocketMsg//WebSocket的消息
    };

    /****************************发送****************************/
    template<class T>//模版重载SendMsg()
    int SendMsg(const T& msg, const CommunicationDirection direction){
        if(role == LocalClient){
            if(direction == UplinkMsg){//本地UI发送上行消息 即本地UI--->远程UI(WebSocket)
                return wsAdapter->SendMsg(msg);
            }else{//本地UI发送下行消息 即本地UI--->Controller(Modbus)
                assert(modbusCmt);
                return modbusCmt->SendMsg(msg);
            }
        }else{
            if(direction == UplinkMsg){//远程UI发送上行消息(不存在)
                assert(false);
                return -1;
            }else{//远程UI发送下行消息 即远程UI--->本地UI(WebSocket)
                return wsAdapter->SendMsg(msg);
            }
        }
    }

    /****************************接收****************************/
    Message CheckMsg(const ReceiveMsgType type);//检查所有消息

    template<class T>//模版重载ReceiveMsg()
    void ReceiveMsg(T& msg, const int type){
        if(role == LocalClient){
            if(type == ModbusMsg){//本地UI接收Modbus消息 即Controller--->本地UI(Modbus)
                assert(modbusCmt);
                modbusCmt->ReceiveMsg(msg);
                SendMsg(msg, UplinkMsg);//本地UI上行转发给远程UI
                return;
            }else{//本地UI接收WebSocket消息 即远程UI--->本地UI(WebSocket)
                wsAdapter->ReceiveMsg(msg);
                SendMsg(msg, DownlinkMsg);//本地UI下行转发给Controller
            }
        }else{
            if(type == ModbusMsg){//远程UI接收Modbus消息(不存在)
                assert(false);
            }else{//远程UI接收Websocket消息 即本地UI--->远程UI(WebSocket)
                return wsAdapter->ReceiveMsg(msg);
            }
        }
    }

public:
    explicit CommunicationMediator();
    virtual ~CommunicationMediator();
private:
    static int role;

private:
    Communicator* modbusCmt;//注意 只有本地UI(LocalClient)使用到Modbus
#ifdef ENABLE_WEBSOCKET_COMMUNICATION
    WebSocketAdapter* wsAdapter;
#else
    WebSocketFakeAdapter* wsAdapter;
#endif
};

#endif // COMMUNICATIONMEDIATOR_H
