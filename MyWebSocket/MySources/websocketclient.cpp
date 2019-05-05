#include "websocketclient.h"

#include "common/printf.h"

#include "MyWebSocket/websocketcommunicator.h"

using websocketpp::lib::placeholders::_1;
using websocketpp::lib::placeholders::_2;
using websocketpp::lib::bind;

WebSocketClient::WebSocketClient(WebSocketCommunicator *wsCmt)
    : wsClient(), wsClientThread(),
      status(Stopped),
      clientConnection(), clientHandler(),
      cmt(wsCmt)
{
    SetLogControl(false);

    PRINTF(LOG_DEBUG, "%s: constructor OK...\n", __func__);
}

WebSocketClient::~WebSocketClient()
{
    PRINTF(LOG_DEBUG, "%s: start...\n", __func__);
    Stop();
}

int WebSocketClient::ConnectServer(const std::string& hostAddress, const uint16_t port)
{
    //convert the address & port to an uri
    const std::string uri = ConstructUri(hostAddress, port);
    PRINTF(LOG_DEBUG, "%s: client would connect to %s\n", __func__, uri.c_str());

    //Initialize Asio Transport
    wsClient.init_asio();
    //for client, run perpetually
    wsClient.start_perpetual();

    //register message handlers for client
    wsClient.set_open_handler(    bind(&WebSocketClient::OnOpenConnection,    this, ::_1) );
    wsClient.set_close_handler(   bind(&WebSocketClient::OnCloseConnection,   this, ::_1) );
    wsClient.set_fail_handler(    bind(&WebSocketClient::OnFailConnection,    this, ::_1) );
    wsClient.set_message_handler( bind(&WebSocketClient::OnReceiveMessage,    this, ::_1, ::_2) );
    wsClient.set_ping_handler(    bind(&WebSocketClient::OnPingMessage,       this, ::_1, ::_2) );
    wsClient.set_pong_handler(    bind(&WebSocketClient::OnPongMessage,       this, ::_1, ::_2) );
    wsClient.set_pong_timeout_handler( bind(&WebSocketClient::OnPongMessageTimeOut, this, ::_1, ::_2) );

    //create a new connection to the converted uri
    websocketpp::lib::error_code ec;
    clientConnection = wsClient.get_connection(uri, ec);
    if(ec){
        PRINTF(LOG_ERR, "%s: could not create connection(%s)\n", __func__, ec.message().c_str());
        return -1;
    }
    //grab a handle for this connection, so that we can talk to it in a thread safe manor after the event loop starts.
    clientHandler = clientConnection->get_handle();

    //requests a connection
    //  Note that connect here only requests a connection.
    //  No network messages are exchanged, until the event loop starts running after wsClient.run()
    wsClient.connect(clientConnection);

    //start another thread to run client
    StartClientThread();

    return 0;
}

void WebSocketClient::Stop()
{
    PRINTF(LOG_DEBUG, "%s: start...\n", __func__);

    if(status == Running){
        status = Stopped;
        PRINTF(LOG_DEBUG, "%s: client would be stopped\n", __func__);

        //stop & close the client
        wsClient.stop_perpetual();
        wsClient.close(clientConnection->get_handle(), websocketpp::close::status::going_away, "client is going down");

        //stop client thread
        StopClientThread();
    }else{
        PRINTF(LOG_DEBUG, "%s: client has alrady been stopped!\n", __func__);
    }
}

void WebSocketClient::SendTextMessage(const std::string& message)
{
    if( !CheckStatus() ){
        return;
    }

    wsClient.send(clientHandler, message, websocketpp::frame::opcode::value::text);
}

void WebSocketClient::SendBinaryMessage(const void *message, size_t len)
{
    if( !CheckStatus() ){
        return;
    }

    wsClient.send(clientHandler, message, len, websocketpp::frame::opcode::value::binary);
}

void WebSocketClient::SendPingMessage(const std::string &message)
{
    if(!CheckStatus()){
        return;
    }

    wsClient.ping(clientHandler, message);
}

void WebSocketClient::Run()
{
    PRINTF(LOG_DEBUG, "WebSocketClient::%s: start...\n", __func__);

    /* run():
     * Start the internal asio io_service run loop
     * this will cause a single connection to be made to the server.
     * this run() will exit when this connection is closed. */
    try {
        wsClient.run();//阻塞式函数
    }catch(websocketpp::exception const & e){
        if(status == Running){
            PRINTF(LOG_ERR, "%s: error exit! catch exception=%s\n", __func__, e.what());
        }
    } catch (...) {
        PRINTF(LOG_ERR, "%s: catch other exception\n", __func__);
    }

    PRINTF(LOG_NOTICE, "%s: client exits...\n", __func__);
}

void WebSocketClient::SetLogControl(bool enableLogger)
{
    // Set logging settings (This is useful for debug)
    wsClient.set_access_channels(enableLogger? websocketpp::log::alevel::all: websocketpp::log::alevel::none);
    wsClient.set_error_channels(enableLogger? websocketpp::log::elevel::all: websocketpp::log::elevel::none);

    //always no frame payload
    wsClient.clear_access_channels(websocketpp::log::alevel::frame_payload);
}

void WebSocketClient::StartClientThread()
{
    PRINTF(LOG_DEBUG, "%s: start...\n", __func__);

    //启动另一个线程运行Run函数
    wsClientThread.reset(new websocketpp::lib::thread(&WebSocketClient::Run, this));
}

void WebSocketClient::StopClientThread()
{
    PRINTF(LOG_DEBUG, "%s: start...\n", __func__);
    wsClientThread->join();
}

std::string WebSocketClient::ConstructUri(const std::string &hostAddress, const uint16_t port)
{
    enum SecurityWebSocket{
        ws =0,//false 目前使用ws
        wss=1,//true wss没有实现
    };
    websocketpp::uri myUri(ws, hostAddress, port, "");
    return myUri.str();
}

bool WebSocketClient::CheckStatus()
{
    if(status == Running){
        return true;
    }else{
        PRINTF(LOG_ERR, "%s: client is not running!\n", __func__);
        return false;
    }
}

void WebSocketClient::OnOpenConnection(websocketpp::connection_hdl handler)
{
    std::string str = GetHandlerInformation(handler, OpenInform);
    PRINTF(LOG_DEBUG, "%s: %s\n", __func__, str.c_str());
    status = Running;

    cmt->OnConnectionEstablished();
}

void WebSocketClient::OnCloseConnection(websocketpp::connection_hdl handler)
{
    std::string str = GetHandlerInformation(handler, CloseInform);
    PRINTF(LOG_DEBUG, "%s: %s\n", __func__, str.c_str());
    status = Stopped;

    cmt->OnConnectionClosed();
}

void WebSocketClient::OnFailConnection(websocketpp::connection_hdl handler)
{
    std::string str = GetHandlerInformation(handler, FailInform);
    PRINTF(LOG_DEBUG, "%s: %s\n", __func__, str.c_str());
}

void WebSocketClient::OnReceiveMessage(websocketpp::connection_hdl handler, WebSocketClient::message_ptr msg)
{
    const int code = msg->get_opcode();//消息类型
    //PRINTF(LOG_DEBUG, "%s: recv code=%d\n", __func__, code);

    switch(code){
    case websocketpp::frame::opcode::text:
        ProcessTextMessage(handler, msg);
        break;
    case websocketpp::frame::opcode::binary:
        ProcessBinaryMessage(handler, msg);
        break;
    default:
        ProcessUnknownMessage(handler, msg);
        return;
    }
}

bool WebSocketClient::OnPingMessage(websocketpp::connection_hdl handler, std::string msg)
{
    std::string str = GetHandlerInformation(handler, PingInform);
    str += ", msg="+msg;
    PRINTF(LOG_DEBUG, "%s: %s\n", __func__, str.c_str());

    return true;
}

bool WebSocketClient::OnPongMessage(websocketpp::connection_hdl handler, std::string msg)
{
    std::string str = GetHandlerInformation(handler, PongInform);
    str += ", msg="+msg;
    PRINTF(LOG_DEBUG, "%s: %s\n", __func__, str.c_str());

    return true;
}

bool WebSocketClient::OnPongMessageTimeOut(websocketpp::connection_hdl handler, std::string msg)
{
    std::string str = GetHandlerInformation(handler, PongTimeOut);
    str += ", msg="+msg;
    PRINTF(LOG_DEBUG, "%s: %s\n", __func__, str.c_str());

    return true;
}

void WebSocketClient::ProcessTextMessage(websocketpp::connection_hdl handler, WebSocketClient::message_ptr msg)
{
    std::string str = msg->get_payload();//消息内容
    PRINTF(LOG_DEBUG, "%s: recv=%s\n", __func__, str.c_str());

    Q_UNUSED(handler);
}

void WebSocketClient::ProcessBinaryMessage(websocketpp::connection_hdl handler, WebSocketClient::message_ptr msg)
{
    std::string str = msg->get_payload();//消息内容
    //PRINTF(LOG_DEBUG, "%s: recv=%s\n", __func__, str.c_str());

    cmt->OnReceiveBinaryMsg(handler, msg->get_payload());
}

void WebSocketClient::ProcessUnknownMessage(websocketpp::connection_hdl handler, WebSocketClient::message_ptr msg)
{
    std::string str = GetHandlerInformation(handler, UnknownMsg);
    str += ", msg="+msg->get_payload();
    PRINTF(LOG_DEBUG, "%s: recv=%s\n", __func__, str.c_str());
}

std::string WebSocketClient::GetHandlerInformation(websocketpp::connection_hdl handler, int informType)
{
    std::string information="unknown";

    WSPPClient::connection_ptr connection = wsClient.get_con_from_hdl(handler);
    if(informType & OpenInform){
        information = "Open: ";
        information += "connects to " + GetConnectionUri(connection);
    }else if(informType & CloseInform){
        information = "Close: from ";
        information += GetConnectionUri(connection)+": ";
        information += connection->get_remote_close_reason();
    }else if(informType & FailInform){
        information = "Fail: from ";
        information += GetConnectionUri(connection)+": ";
        information += connection->get_ec().message();
    }else if(informType & PingInform){
        information = "Ping: from ";
        information += GetConnectionUri(connection);
    }else if(informType & PongInform){
        information = "Pong: from ";
        information += GetConnectionUri(connection);
    }else if(informType & PongTimeOut){
        information = "PongTimeOut: from ";
        information += GetConnectionUri(connection)+ ": ";
        information += connection->get_ec().message();
    }else if(informType & UnknownMsg){
        information = "Unkown: from ";
        information += GetConnectionUri(connection);
    }

    return information;
}

std::string WebSocketClient::GetConnectionUri(WSPPClient::connection_ptr connection)
{
    return connection->get_uri()->get_host()+":"+connection->get_uri()->get_port_str();
}
