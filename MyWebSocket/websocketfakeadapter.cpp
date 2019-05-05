#include "websocketfakeadapter.h"

#include <QtGlobal>

#include "common/printf.h"

WebSocketFakeAdapter::WebSocketFakeAdapter(const std::string &groupId, const std::string &groupPassword, const std::string &rsaPublicKeyFile)
{
    Q_UNUSED(groupId);
    Q_UNUSED(groupPassword);
    Q_UNUSED(rsaPublicKeyFile);

    PRINTF(LOG_DEBUG, "%s: use fake adapter to communicate!\n", __func__);
}

WebSocketFakeAdapter::~WebSocketFakeAdapter()
{
    //nop
}

void WebSocketFakeAdapter::Start(const std::string &serverIp, const uint16_t port)
{
    Q_UNUSED(serverIp);
    Q_UNUSED(port);
}

void WebSocketFakeAdapter::Stop()
{
    //nop
}

int WebSocketFakeAdapter::CheckMsg()
{
    return -1;//no new message
}
