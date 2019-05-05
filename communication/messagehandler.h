#ifndef MESSAGEHANDLER_H
#define MESSAGEHANDLER_H

#include <tr1/functional>

#include "communication/MsgQueue.h"

struct MessageHandler {
    typedef std::tr1::function<void (const Message&, const int)> tr1MsgHandler;
    tr1MsgHandler handler;
};

#endif // MESSAGEHANDLER_H
