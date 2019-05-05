#ifndef MODBUSQUEUE_H
#define MODBUSQUEUE_H

#include "communication/libmodbus/modbus/modbus.h"

#include "communication/MsgQueue.h"

class ModbusQueue
{
public:
    ModbusQueue(const int id, const char* comNum);
    enum Identidy{
        Master = 1,
        Slaver
    };
    virtual ~ModbusQueue();

    void Run();

    int SendMsg2Queue(const int type, const int addr, const int nregs);
    Message CheckMsg();

private:
    int Init(const char* comNum);

    /* Master */
    /* 主机主动发送消息 写入Modbu寄存器 */
    void MasterHandleSendQueue();
    /* 主机接收消息 读取并检查Modbus标志位 */
    void MasterReceiveMessage();

    /* Slaver */
    /* 从机对主机消息进行应答 */
    void SlaverReply();
    /* 从机被动发送消息 将Modbus中的tab_bits(slaver_tab_bits)置一 等待主机轮询 */
    void SlaverHandleSendQueue();
    /* 从机接收消息 检查Modbus中的tab_bits(slaver_tab_bits) 主机轮询把该位置一*/
    void SlaverReceiveMessage();

    /* Modbus Operation */
    void WriteRegisters(int addr, int nb_left);
    void ReadRegisters(int addr, int nb_left);

    void SetBit(int addr);
    void ClearBit(int addr);
    void WriteBit(int addr, int status);
    void ReadBits(int addr, int nb, uint8_t *dest);

private:
    const int mIdentity;

    const int start_tab_bits_index;
    const int end_tab_bits_index;
    const int tab_bits_len;

    volatile bool status;//线程共享
    volatile bool statusReturn;//线程共享

    enum tagStatus{
        Running,
        Stopped
    };

    MsgQueue sendQueue;
    MsgQueue recvQueue;

    modbus_t *ctx;

    unsigned char *slaver_tab_bits;
    modbus_mapping_t *slaver_mb_mapping;
    uint8_t slaver_query[MODBUS_TCP_MAX_ADU_LENGTH];
};

#endif // MODBUSQUEUE_H
