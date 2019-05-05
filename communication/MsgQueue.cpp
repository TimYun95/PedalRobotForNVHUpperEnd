#include "MsgQueue.h"

const int maxQueueLength = 20;

MsgQueue::MsgQueue()
    :length(maxQueueLength)
{
    array = new Message[length];
	head=tail=0;
}

MsgQueue::~MsgQueue()
{
    delete[] array;//array是个数组
}

int MsgQueue::push(const Message& item)
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

const Message& MsgQueue::front()
{
    return array[head];
}

void MsgQueue::pop()
{
    if(empty()){
        return;
    }

    head = (head+1)%length;
}

int MsgQueue::size()
{
	int msize=0;
    if (tail>head){
        msize=tail-head;
    }else if(tail==head){
        msize=0;
    }else if(tail<head){
        msize=length - (head-tail);
    }

	return msize;
}

bool MsgQueue::empty()
{
	return head==tail;
}
