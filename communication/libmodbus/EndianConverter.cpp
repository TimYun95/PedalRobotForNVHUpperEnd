#include "EndianConverter.h"

EndianConverter::EndianConverter()
{
    Init();
}

EndianConverter::~EndianConverter()
{
}

unsigned char *EndianConverter::Data()
{
    return &data[0];
}

int EndianConverter::length()
{
    return data.size();
}

EndianConverter &EndianConverter::toModbus()
{
    //本地 到 Modbus(大端)
    if(localEndian==LittleEndian){
        /* convert to big-endian */
        ChangeEndian();
    }
    return *this;
}

EndianConverter &EndianConverter::toLocal()
{
    //Modbus(大端) 到 本地
    if(localEndian==LittleEndian){
        /* convert to little-endian */
        ChangeEndian();
    }
    return *this;
}

void EndianConverter::loadFromRegisters(void *mem, int nregs)
{
    parse_index=0;
    memcpy(&data[0], mem, nregs*sizeof(short));
}

EndianConverter &EndianConverter::addInt(int n)
{
    addElement(&n, sizeof(int));
    return *this;
}

EndianConverter &EndianConverter::addDouble(double n)
{
    addElement(&n, sizeof(double));
    return *this;
}

EndianConverter &EndianConverter::addShort(short n)
{
    addElement(&n, sizeof(short));
    return *this;
}

EndianConverter &EndianConverter::ai(int n)
{
    for(int i=0; i<n; i++){
        addInt();
    }
    return *this;
}

EndianConverter &EndianConverter::ad(int n)
{
    for(int i=0; i<n; i++){
        addDouble();
    }
    return *this;
}

EndianConverter &EndianConverter::as(int n)
{
    for(int i=0;i<n;i++){
        addShort();
    }
    return *this;
}

int EndianConverter::parseInt()
{
    int n;
    memcpy(&n,&data[parse_index],sizeof(int));
    parse_index+=sizeof(int);
    return n;
}

double EndianConverter::parseDouble()
{
    double n;
    memcpy(&n,&data[parse_index],sizeof(double));
    parse_index+=sizeof(double);
    return n;
}

short EndianConverter::parseShort()
{
    short n;
    memcpy(&n,&data[parse_index],sizeof(short));
    parse_index+=sizeof(short);
    return n;
}

void EndianConverter::Init()
{
    data.clear();
    bytes_of_element.clear();
    parse_index=0;
    localEndian = GetLocalEndian();
}

int EndianConverter::GetLocalEndian()
{
    union myUnion{
        int a;
        char b;
    }c;
    c.a=1;

    return (c.b==1 ? LittleEndian : BigEndian);
}

/* 数据报文data的大小端转换
 * data[i]!=第i个数据(!!!不等于!!!)
 * bytes_of_element[i]=第i个数据的大小
 * eg.
 *  data=[1,2,3,4,5,6,7,8,9](3个数12,345,6789)
 *  bytes_of_element=[2,3,4](3个数的长度)
 *  转换为[2,1,5,4,3,9,8,7,6](3个数21,543,9876)
 *  index=0 bytes=2; [1,2]
 *  index=2 bytes=3; [3,4,5]
 *  index=5 bytes=4; [6,7,8,9] */
void EndianConverter::ChangeEndian()
{
    /* std::vector的内存空间连续
     * 故ChangeEndian()中可以使用取地址和地址偏移操作(&data[0], puc+index)
     * The elements of a vector are stored contiguously,
     * meaning that if v is a vector<T, Allocator> where T is some type other than bool,
     * then it obeys the identity &v[n] == &v[0] + n for all 0 <= n < v.size(). */

    int index = 0;
    unsigned char* puc = &data[0];
    for(size_t i=0; i<bytes_of_element.size(); ++i){//对于每个数据
        int bytes = bytes_of_element[i];//该数据大小
        SwapBytes(puc+index, bytes);
        index += bytes;
    }
}

void EndianConverter::SwapBytes(void *pValue, int bytes)
{
    char* pc = static_cast<char*>(pValue);

    char dest[bytes];
    for(int i=0; i<bytes; ++i){
        dest[i] = pc[bytes-1-i];
    }
    memcpy(pValue, dest, bytes);
}

void EndianConverter::addElement(void *p, int nb)
{
    unsigned char* puc = static_cast<unsigned char*>(p);
    for(int i=0; i<nb; ++i){//保存数据的每个字节
        data.push_back(puc[i]);
    }
    bytes_of_element.push_back(nb);//保存数据的长度
}
