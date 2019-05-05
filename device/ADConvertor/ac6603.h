/////////////////////////////////////////////////////////////////
//AC6603 include files
//wwlab (c)2006/11   by york wang

//#define HANDLE void*

//for DIO
extern "C" long __declspec(dllimport) __stdcall AC6603_DI(void* hHandle);
extern "C" long __declspec(dllimport) __stdcall AC6603_DO(void* hHandle,long iodata);

//for AD
extern "C" long __declspec(dllimport) __stdcall AC6603_AD(void* hHandle,long channel,long gain);
extern "C" long __declspec(dllimport) __stdcall AC6603_CAL(void* hDevice);

extern "C" void* __declspec(dllimport) __stdcall AC6603_OpenDevice(long DeviceNum);
extern "C" long __declspec(dllimport) __stdcall AC6603_CloseDevice(void* hHandle);



//--------------------------------------not for user ------------------------------------------
extern "C" long __declspec(dllimport) __stdcall AC6603_inb(void* hDevice,long adr);
extern "C" long __declspec(dllimport) __stdcall  AC6603_outb(void* hDevice,long adr,long data1);


extern "C" long __declspec(dllimport)  __stdcall AC6603_RDCAL(void* hDevice,long *caldata);
extern "C" long __declspec(dllimport)  __stdcall AC6603_WRCAL(void* hDevice,long *caldata);
extern "C" long __declspec(dllimport) __stdcall  AC6603_sam(void* hDevice,long ch,long *cdata);
