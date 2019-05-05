#ifndef WEBSOCKETMSGQUEUE_H
#define WEBSOCKETMSGQUEUE_H

#include "MyWebSocket/mytypedefine.h"

/* WebSocket的消息队列(回环队列)
 * 由于WebSocket的加密通讯, string可能被\0截断
 * 所以这里使用int push(const char* item, const size_t len); 指定消息长度的push
 * */

class WebSocketMsgQueue {
public:
    explicit WebSocketMsgQueue(int maxQueueLength = 20);
    virtual ~WebSocketMsgQueue();

    int push(const std::string& item);
    int push(const char* item, const size_t len);
    int push(const shared_string_ptr& item);
    shared_string_ptr front();
    void pop();

    int size();
    bool empty();

private:
    /* 环形队列 保存智能指针 */
    shared_string_ptr *array;
    int head, tail;
    const int length;
};

#endif // WEBSOCKETMSGQUEUE_H
