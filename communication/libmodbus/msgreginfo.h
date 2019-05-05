#ifndef MSGREGINFO_H
#define MSGREGINFO_H

#define TAB_BITS_LEN 100
#define TAB_REGISTERS_LEN 20*1024

extern unsigned short* tab_registers;//tab_registers寄存器中存储消息数据

struct message_info {
    int msgType;
    int addr;
    int nregs;
};

extern message_info msgInfo[TAB_BITS_LEN];

#endif // MSGREGINFO_H
