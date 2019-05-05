#include "websocketcommunicator.h"

#include <thread>

#include "common/printf.h"

#include "MyWebSocket/MySources/websocketmsgtypes.h"
#include "MyWebSocket/MySources/websocketmessage.h"
#include "MyWebSocket/MySources/websocketconfigurationmessage.h"

//加密算法
#include "MyWebSocket/MySources/OpenSSLWrapper/rsaencryptor.h"
#include "MyWebSocket/MySources/OpenSSLWrapper/aesencryptor.h"

typedef std::lock_guard<std::mutex> MutexLock;

WebSocketCommunicator::WebSocketCommunicator(const std::string& groupId, const std::string& groupPassword, const std::string& rsaPublicKeyFile)
    : wsClient(this),
      recvQueue(), recvMutex(),
      sendQueue(), sendMutex(), sendSem(),
      status(Stopped),
      wsGroupId(groupId), wsGroupPassword(groupPassword)
{
    RSAEncryptor::ReadPublicKey(rsaPublicKeyFile);
    AESEncryptor::SetDefaultInitVec();
}

WebSocketCommunicator::~WebSocketCommunicator()
{
    Stop();
    RSAEncryptor::FreeKeys();
}

void WebSocketCommunicator::Start(const std::string &serverIp, const uint16_t port)
{
    if(status==Error || status==Stopped){
        int ret = wsClient.ConnectServer(serverIp, port);
        if(ret==-1){
            status = Error;
            PRINTF(LOG_ERR, "%s: client start error!\n", __func__);
        }else{
            status = Established;
            PRINTF(LOG_DEBUG, "%s: client wait for password\n", __func__);
        }
    }
}

void WebSocketCommunicator::Stop()
{
    if(status!=Error && status!=Stopped){
        SendCloseMsg();
        wsClient.Stop();

        status = Stopped;
        sendSem.notify();//强制wait的退出 见BasicSemaphore的说明
    }
}

void WebSocketCommunicator::SendWebSocketMsg(const std::string& msg)
{
    if( !CheckConnection() ){
        PRINTF(LOG_ERR, "%s: error, connection is not online...!\n", __func__);
        return;
    }

    //加密消息
    std::string encryptedMsg;
    int ret = AESEncryptor::Encrypt(msg, encryptedMsg);
    if(ret == -1){//params for aes are not ready
        return;
    }

    //格式化消息
    WebSocketMessage wsMsg;
    SetSrcDstGroupId(wsMsg);
    wsMsg.SetMsgContent(WSMsgTypes::WS_FORWARD_2ServerClient, encryptedMsg.c_str(), encryptedMsg.length());
    std::string sentMsg;
    wsMsg.Format(sentMsg);

    //放入发送队列 由Run()进行具体的消息发送
    do{
        MutexLock lock(sendMutex);
        sendQueue.push(sentMsg.c_str(), sentMsg.length());
    }while(0);
    sendSem.notify();
}

bool WebSocketCommunicator::CheckRecvMsg(shared_string_ptr& msg)
{
    MutexLock lock(recvMutex);
    if(recvQueue.size() > 0){
        msg = recvQueue.front();
        recvQueue.pop();
        return true;
    }else{
        return false;
    }
}

void WebSocketCommunicator::Run()
{
    //Run is started after client receives the configuration from server(see ReceiveConfMsg())
    PRINTF(LOG_DEBUG, "WebSocketCommunicator::%s: start...\n", __func__);

    while(status == Running){
        //wait msg from send queue
        sendSem.wait();

        //get the msg from send queue
        shared_string_ptr msg;
        do{
            MutexLock lock(sendMutex);
            if(sendQueue.size() > 0){
                msg = sendQueue.front();
                sendQueue.pop();
            }
        }while(0);

        //send this msg via WebSocket
        if(msg.use_count() != 0){//msg DOES get content from queue
            wsClient.SendBinaryMessage(msg->c_str(), msg->length());
        }
    }

    PRINTF(LOG_ALERT, "WebSocketCommunicator::%s: exit...\n", __func__);
}

void WebSocketCommunicator::OnConnectionEstablished()
{
    //WS连接建立后的回调函数
    PRINTF(LOG_DEBUG, "%s...\n", __func__);

    /* client端 连接建立后立即发送群组ID和密码的登录信息(RSA加密)
     * server端 接收客户端的发送的群组ID和密码
     * 1. 若存在该群ID 则检查群组密码
     *     密码正确 将新的客户加入群组+回传群组内部的AES密钥
     *     密码错误 关闭该连接
     * 2. 若不存在该群ID 则新建群组ID并保存密码
     *     同时保存一份随机生成的32B的AES密码并回传给客户端(该密码加入群组的数据结构) */

    //客户端使用RSA公钥加密登录信息(群组ID和密码)
    WebSocketConfigurationMessage wsConf(wsGroupId, wsGroupPassword);
    const std::string encryptedConfMsg = wsConf.Serialize();

    if(encryptedConfMsg.empty()){
        PRINTF(LOG_ERR, "%s: wsConf serialize error!\n", __func__);
        status = Error;
        return;
    }else{
        //发送加密后的登录信息至服务器
        WebSocketMessage wsMsg;
        SetSrcDstGroupId(wsMsg);
        wsMsg.SetMsgContent(WSMsgTypes::WS_CONFIG_2Server, encryptedConfMsg.c_str(), encryptedConfMsg.length());
        std::string sentMsg;
        wsMsg.Format(sentMsg);

        wsClient.SendBinaryMessage(sentMsg.c_str(), sentMsg.length());
        PRINTF(LOG_DEBUG, "%s: wsConf is sent(id=%s password=%s)!\n", __func__, wsGroupId.c_str(), wsGroupPassword.c_str());
        status = WaitForAesKey;
    }
}

void WebSocketCommunicator::OnConnectionClosed()
{
    PRINTF(LOG_NOTICE, "%s: ws connection is closed!\n", __func__);
    status = Stopped;
}

void WebSocketCommunicator::OnReceiveBinaryMsg(websocketpp::connection_hdl handler, const std::string &msg)
{
    Q_UNUSED(handler);

    WebSocketMessage wsMsg;
    wsMsg.Deformat(msg);

    switch(wsMsg.GetMsgType()){
    case WSMsgTypes::WS_CONFIG_2Client:
        ReceiveConfMsg( wsMsg.GetMsgContent() );
        break;
    case WSMsgTypes::WS_FORWARD_2ServerClient:
        ReceiveForwardMsg( wsMsg );
        break;
    }
}

void WebSocketCommunicator::ReceiveConfMsg(const std::string &encryptedKey)
{
    //接收到服务器发送的配置消息(该配置消息包含群组内通讯的AES密钥)
    //客户端使用RSA公钥进行解密 获得群组客户间通讯的群组AES密钥
    std::string groupKey;
    RSAEncryptor::DecryptViaPublicKey(encryptedKey, groupKey);
    PRINTF(LOG_DEBUG, "%s: groupKey=%s\n", __func__, groupKey.c_str());

    //设置群组通讯的AES密钥
    AESEncryptor::SetKey(groupKey);
    status = Running;

    //至此 才可以启动发送消息的线程
    std::thread runThread(&WebSocketCommunicator::Run, this);
    runThread.detach();//分离子线程 子线程执行完后会自己释放线程资源

    PRINTF(LOG_DEBUG, "%s: WebSocket is configured OK. WS Connection is online...\n", __func__);
}

void WebSocketCommunicator::ReceiveForwardMsg(const WebSocketMessage &wsMsg)
{
    //接收到服务器转发的群组消息
    //客户端使用AES密钥解密 获得消息内容
    std::string decryptedMsg;
    AESEncryptor::Decrypt(wsMsg.GetMsgContent(), decryptedMsg);
    PRINTF(LOG_DEBUG, "%s: recv forward msg=%s\n", __func__, decryptedMsg.c_str());

    //消息放入接收队列
    MutexLock lock(recvMutex);
    recvQueue.push(decryptedMsg.c_str(), decryptedMsg.length());
}

void WebSocketCommunicator::SendCloseMsg()
{
    //发送关闭消息 使得服务器可以从群组中删除该客户
    WebSocketMessage wsMsg;
    SetSrcDstGroupId(wsMsg);
    wsMsg.SetMsgContent(WSMsgTypes::WS_CLOSE_2Server, wsGroupId.c_str(), wsGroupId.length());

    std::string sentMsg;
    wsMsg.Format(sentMsg);

    wsClient.SendBinaryMessage(sentMsg.c_str(), sentMsg.length());
}

void WebSocketCommunicator::SetSrcDstGroupId(WebSocketMessage &wsMsg)
{
    wsMsg.SetSrcGroupId(wsGroupId);
    wsMsg.SetDstGroupId(wsGroupId);
}

bool WebSocketCommunicator::CheckConnection()
{
    if(status==Running){
        return true;
    }

    return false;
}
