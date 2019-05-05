#ifndef ENDIAN_CONVERTER
#define ENDIAN_CONVERTER

/* 在MODBUS中,数据需要组织成数据报文
 * MODBUS报文中的数据都是大端模式 即低地址存高位,高地址存低位
 * EndianConverter完成 '本地大小端-Modbus大端'的相互转换 */

#include <string.h>

#include <vector>
#include <tr1/memory>

class EndianConverter {
public:
    EndianConverter();
    ~EndianConverter();

    unsigned char* Data();//数据报文的每个字节内容
    int length();//数据报文的字节数

    /* 本地/Modbus的大小端转换 */
    EndianConverter& toModbus();//用于发送
    EndianConverter& toLocal();//用于接收

    /****************** write into modbus ******************/
    /*1) addXXX() 加入数据报文内容
     *2) toModbus() 转换为Modus字节序
     *3) tab_registers+msgInfo[msgType].addr 写入数据报文 */
    EndianConverter& addInt(int n=0);
    EndianConverter& addDouble(double n=0.0);
    EndianConverter& addShort(short n=0);

    template <typename T, int asize>
    EndianConverter& addIntArray(T (&arr)[asize])
    {
        for(int i=0;i<asize;i++){
            addInt(arr[i]);
        }
        return *this;
    }
    template <typename T, int asize>
    EndianConverter& addDoubleArray(T (&arr)[asize])
    {
        for(int i=0;i<asize;i++){
            addDouble(arr[i]);
        }
        return *this;
    }
    template <typename T, int asize>
    EndianConverter& addShortArray(T (&arr)[asize])
    {
        for(int i=0;i<asize;i++){
            addShort(arr[i]);
        }
        return *this;
    }

    EndianConverter& ai(int n);/*add n int numbers to hold positions*/
    EndianConverter& ad(int n);/*add n double numbers to hold positions*/
    EndianConverter& as(int n);/*add n short numbers to hold positions*/

    /****************** read from modbus ******************/
    /*1) addXXX() 加入数据报文占位
     *2) loadFromRegisters() 读取数据报文
     *3) toLocal() 转换为本地字节序
     *4) parseXXX() 读取数据内容 */
    void loadFromRegisters(void* mem, int nregs);/* load data from registers (big-endian) */
    int parseInt();
    double parseDouble();
    short parseShort();

private:
    void Init();
    int GetLocalEndian();

    void ChangeEndian();//数据报文data的大小端转换
    void SwapBytes(void* pValue, int bytes);//从pValue开始的bytes个字节 交换位置
    void addElement(void* p, int nb);//以字节为单位 保存数据到data中

private:
    /* 数据报文 保存每个数据的每个字节 */
    std::vector<unsigned char> data;
    /* 每个数据的字节数 */
    std::vector<int> bytes_of_element;

    int parse_index;

    int localEndian;
    enum Endian{
        LittleEndian,//小端
        BigEndian//大端
    };
};

#endif
