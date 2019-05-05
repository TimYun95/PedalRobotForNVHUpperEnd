#include "modbusqueue.h"

#include <errno.h>
#include <string.h>
#include <assert.h>

#include "common/printf.h"
#include "common/message.h"

#include "communication/libmodbus/msgreginfo.h"

void* modbusqueue_thread_func(void* arg)
{
    //线程工作
    ModbusQueue* modbus=(ModbusQueue*)arg;
    modbus->Run();

    return NULL;
}

ModbusQueue::ModbusQueue(const int id, const char* comNum)
    : mIdentity(id),
      start_tab_bits_index(id==Master? 0: TAB_BITS_LEN/2),
      end_tab_bits_index(id==Master? TAB_BITS_LEN/2: TAB_BITS_LEN),
      tab_bits_len(end_tab_bits_index - start_tab_bits_index)
{
    ctx = NULL;
    slaver_tab_bits = NULL;
    slaver_mb_mapping = NULL;

    Init(comNum);
}

ModbusQueue::~ModbusQueue()
{
    status = Stopped;
    while(statusReturn == Running)
    {
        ;
    }

    if(mIdentity == Master && tab_registers != NULL){
        delete [] tab_registers;
        tab_registers = NULL;
    }
    if(slaver_mb_mapping != NULL){
        modbus_mapping_free(slaver_mb_mapping);
    }
    if(ctx != NULL){
        modbus_close(ctx);
        modbus_free(ctx);
    }
}

void ModbusQueue::Run()
{
    PRINTF(LOG_DEBUG, "%s starts\n", __func__);
    status = Running;
    statusReturn = Running;

    switch(mIdentity){
    case Master:
        while(status == Running){
            MasterReceiveMessage();
            MasterHandleSendQueue();
        }
        break;
    case Slaver:
        while(status == Running){
            SlaverReply();
            SlaverReceiveMessage();
            SlaverHandleSendQueue();
        }
        break;
    default:
        PRINTF(LOG_ERR, "%s: unknown identity(%d)\n", __func__, mIdentity);
        break;
    }

    statusReturn = Stopped;
    PRINTF(LOG_ERR, "%s: Quit the loop\n", __func__);
}

int ModbusQueue::SendMsg2Queue(const int type, const int addr, const int nregs)
{
    //构造队列数据 并 压入队列
    Message msg(type, addr, nregs);
    if( sendQueue.push(msg) == -1){
        PRINTF(LOG_NOTICE, "%s: push msg(%d) into sendQueue error. Queue is full!\n", __func__, type);
        return -1;
    }
    return 0;
}

Message ModbusQueue::CheckMsg()
{
    //Modbus无新接收到的消息
    if(recvQueue.empty()){
        return Message();
    }

    //队列中取一个消息返回
    Message msg = recvQueue.front();
    recvQueue.pop();
    return msg;
}

int ModbusQueue::Init(const char *comNum)
{
    PRINTF(LOG_DEBUG, "%s: Initializing...\n", __func__);

    //新建ModBus连接
    switch(mIdentity){
    case Master:
        if(comNum ==NULL){
            PRINTF(LOG_ERR, "%s: device(NULL) cannot be initialized as master\n", __func__);
            return -1;
        }else{
            PRINTF(LOG_DEBUG, "%s: initialize as master(%s)\n", __func__, comNum);
        }

        ctx = modbus_new_rtu(comNum, 115200, 'N', 8, 1);//COMX变化 取决于PC的设备号
        modbus_set_slave(ctx, Slaver);
        //Modbus寄存器配置
        tab_registers = new unsigned short[TAB_REGISTERS_LEN];
        memset(tab_registers, 0, TAB_REGISTERS_LEN);
        break;
    case Slaver:
        PRINTF(LOG_DEBUG, "%s: initialize as slaver\n", __func__);

        ctx = modbus_new_rtu(comNum, 115200, 'N', 8, 1);
        modbus_set_slave(ctx, Slaver);
        //Modbus寄存器配置
        slaver_mb_mapping = modbus_mapping_new(TAB_BITS_LEN, 0, TAB_REGISTERS_LEN, 0);
        if(slaver_mb_mapping==NULL){
            PRINTF(LOG_ERR, "%s: Failed to allocate the mb_mapping(%s)\n", __func__, modbus_strerror(errno));
            return -1;
        }
        slaver_tab_bits=slaver_mb_mapping->tab_bits;
        tab_registers=slaver_mb_mapping->tab_registers;
        break;
    default:
        PRINTF(LOG_ERR, "%s: unknown identity(%d)\n", __func__, mIdentity);
        return -1;
    }

    //连接ModBus
    if(modbus_connect(ctx) == -1){
        PRINTF(LOG_ERR, "%s: modbus_connect error(%s)\n", __func__, modbus_strerror(errno));
        return -1;
    }
    PRINTF(LOG_DEBUG, "%s: Initializing...OK\n", __func__);

    //创建线程
    pthread_t tid;
    pthread_create(&tid, NULL, modbusqueue_thread_func, this);

    return 0;
}

void ModbusQueue::MasterHandleSendQueue()
{
    if(!sendQueue.empty()){
        //取一个消息
        Message msg = sendQueue.front();
        sendQueue.pop();

        //数据写到对应的Modbus寄存器
        WriteRegisters(msg.GetAddr(), msg.GetNregs());
        //设置标志位
        SetBit(msg.GetType());
    }
}

void ModbusQueue::MasterReceiveMessage()
{
    //读取全部标志位
    unsigned char master_tab_bits[tab_bits_len];
    memset(master_tab_bits, 0, sizeof(master_tab_bits));
    ReadBits(start_tab_bits_index, tab_bits_len, master_tab_bits);

    //遍历全部标志位
    for(int i=start_tab_bits_index; i<end_tab_bits_index; ++i){
        if(master_tab_bits[i] != 0){//查找到新消息
            //清空标志位
            ClearBit(i);
            //读取Modbus寄存器的数据(若该消息春在对应的数据内容)
            if(msgInfo[i].nregs != 0){
                ReadRegisters(msgInfo[i].addr, msgInfo[i].nregs);
            }

            //构造消息并放入队列
            Message msg(msgInfo[i].msgType, msgInfo[i].addr, msgInfo[i].nregs);
            recvQueue.push(msg);
        }
    }
}

void ModbusQueue::SlaverReply()
{
    int rc = modbus_receive(ctx, slaver_query);
    if(rc>0){
        modbus_reply(ctx, slaver_query, rc, slaver_mb_mapping);
    }else{
        PRINTF(LOG_ERR, "Connection closed by the client or server(%s).\n", modbus_strerror(errno));
        return;
    }
}

void ModbusQueue::SlaverHandleSendQueue()
{
    if(!sendQueue.empty()){
        //取一个消息
        Message msg = sendQueue.front();
        sendQueue.pop();

        //设置标志位
        slaver_tab_bits[msg.GetType()]=1;
    }
}

void ModbusQueue::SlaverReceiveMessage()
{
    //遍历全部标志位
    for(int i=start_tab_bits_index; i<end_tab_bits_index; ++i){
        if(slaver_tab_bits[i] != 0){//查找到新消息
            //清空标志位
            slaver_tab_bits[i]=0;

            Message msg(msgInfo[i].msgType, msgInfo[i].addr, msgInfo[i].nregs);
            recvQueue.push(msg);
        }
    }
}

void ModbusQueue::WriteRegisters(int addr, int nb_left)
{
    int rc;
    while(nb_left > 0){
        int nb = nb_left>100 ? 100 : nb_left;
        while((rc = modbus_write_registers(ctx, addr, nb, &tab_registers[addr])) != nb){
#ifdef ENABLE_MODBUS_WRITE_READ_ERROR_DEBUG
            PRINTF(LOG_ERR, "ERROR modbus_write_registers (%d)\n",rc);
            PRINTF(LOG_ERR, "Address = %d, nb = %d\n",addr,nb_left);
#endif
        }//write OK!

        nb_left -= nb;
        addr += nb;
    }
}

void ModbusQueue::ReadRegisters(int addr, int nb_left)
{
    int rc;
    while(nb_left>0){
        int nb = nb_left>100 ? 100 : nb_left;
        while((rc = modbus_read_registers(ctx, addr, nb, &tab_registers[addr])) != nb){
#ifdef ENABLE_MODBUS_WRITE_READ_ERROR_DEBUG
            PRINTF(LOG_ERR, "ERROR modbus_read_registers (%d)\n",rc);
            PRINTF(LOG_ERR, "Address = %d, nb = %d\n",addr,nb_left);
#endif
        }//read OK!

        nb_left -= nb;
        addr += nb;
    }
}

void ModbusQueue::SetBit(int addr)
{
    WriteBit(addr, 1);
}

void ModbusQueue::ClearBit(int addr)
{
    WriteBit(addr, 0);
}

void ModbusQueue::WriteBit(int addr, int status)
{
    while(modbus_write_bit(ctx, addr, status) != 1){
#ifdef ENABLE_MODBUS_WRITE_READ_ERROR_DEBUG
        PRINTF(LOG_ERR, "ERROR modbus_write_bit, addr: %d\n",addr);
#endif
    }//write OK!
}

void ModbusQueue::ReadBits(int addr, int nb, uint8_t *dest)
{
    int rc;
    while((rc = modbus_read_bits(ctx, addr, nb, dest)) != nb){
#ifdef ENABLE_MODBUS_WRITE_READ_ERROR_DEBUG
        PRINTF(LOG_ERR, "ERROR modbus_read_bits (%d)\n", rc);
        PRINTF(LOG_ERR, "address= %d\n", addr);
#endif
    }//read OK!
}

