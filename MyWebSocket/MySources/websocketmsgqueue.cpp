#include "websocketmsgqueue.h"

#include <cassert>

WebSocketMsgQueue::WebSocketMsgQueue(int maxQueueLength)
    : array(NULL), head(0), tail(0), length(maxQueueLength)
{
    array = new shared_string_ptr[length];
}

WebSocketMsgQueue::~WebSocketMsgQueue()
{
    delete[] array;//array是个数组
}

int WebSocketMsgQueue::push(const std::string &item)
{
    return push( shared_string_ptr(new std::string(item)) );
}

int WebSocketMsgQueue::push(const char *item, const size_t len)
{
    return push( shared_string_ptr(new std::string(item, len)) );
}

int WebSocketMsgQueue::push(const shared_string_ptr& item)
{
    if(size() >= length-1){
        //=length: 队列满
        //=length-1: 只有1个空位 但是不能放入元素(若放入 则head=tail size()判断队列为空)
        return -1;
    }

    array[tail] = item;
    tail = (tail+1)%length;
	return 0;
}

shared_string_ptr WebSocketMsgQueue::front()
{
    if(empty()){
        assert(false);
    }

    return array[head];
}

void WebSocketMsgQueue::pop()
{
    if(empty()){
        return;
    }

    array[head].reset();//智能指针的引用计数-1
    head = (head+1)%length;
}

int WebSocketMsgQueue::size()
{
    int msize = 0;
    if(tail > head){
        msize = tail-head;
    }else if(tail == head){
        msize = 0;
    }else if(tail < head){
        msize = length - (head-tail);
    }

	return msize;
}

bool WebSocketMsgQueue::empty()
{
    return (head==tail);
}
