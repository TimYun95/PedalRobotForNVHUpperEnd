#ifndef MSGQUEUE_H
#define MSGQUEUE_H

#include <string>

struct Message{
public:
    Message(): isNull(true){}
    Message(int _msgType, int _addr, int _nregs)
        : msgType(_msgType), addr(_addr), nregs(_nregs), isNull(false){}

    bool IsNull() const {return isNull;}
    int GetType() const {return msgType;}
    int GetAddr() const {return addr;}
    int GetNregs() const {return nregs;}//number of registors数据所需的寄存器数量

private:
    int msgType;
    int addr;
    int nregs;
    bool isNull;//指示是否为空消息
};

class MsgQueue {
public:
	MsgQueue();
    virtual ~MsgQueue();

    int push(const Message& item);
    const Message& front();
    void pop();

    int size();
    bool empty();

private:
    Message* array;
    int head,tail;/*head指向第一个元素 tail指向最后一个元素之后的空位置*/
    int length; /* the maximum length of the queue */

};

#endif
