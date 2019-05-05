#ifndef MP412_H
#define MP412_H

#include <windows.h>

/***************************** Open/Close *****************************/
//opem MP412
extern "C" HANDLE __declspec(dllimport)  __stdcall MP412_OpenDevice(__int32 dev_num);
//close device
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_CloseDevice(HANDLE hDevice1);



/***************************** get board info *****************************/
//model or type in *bStr
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_Info(HANDLE hDevice,char *modle);



/***************************** AD *****************************/
//sam a single channel with average times=naver
//ch=0-15 , selected input
//gain: selected input range, 0,1,2,3 via: 10/5/2.5/1.25V, 4,5,6,7 via: B10/B5/B2.5/B1.25V
//sidi: not using by MP412
//return data=0-4095, 12bit ad data, if -1 error
//naver: average times, =0-100, if 0 aver times=1
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_AD(HANDLE hDevice,__int32 ch,__int32 gain,__int32 sidi,__int32 naver);

//Calibrate AD for Gain=gain, return zero data zdata, gain data: gdata
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_CAL(HANDLE hDevice);

//counting ad's voltage
extern "C" double __declspec(dllimport) __stdcall MP412_ADV(__int32 adg,__int32 addata);

/***************************** tad mode 快速采样模式 *****************************/
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_TAD_Read(HANDLE hDevice, __int32 *addata,__int32 rlen);

//return fifo's ad data length
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_TAD_Poll(HANDLE hDevice);

//start tad
//tdata =50 - 65536 , base clk 1uS
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_TAD(HANDLE hDevice,__int32 stch,__int32 endch,__int32 gain,__int32 sidi,__int32 tdata);

//stop AD
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_TAD_Stop(HANDLE hDevice);



/***************************** DIO *****************************/
//get 16bit DI data, data return by MP412_DI
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_DI(HANDLE hDevice);
//set 16bit DO data=dodata
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_DO(HANDLE hDevice,__int32 dodata);



/***************************** EEPROM *****************************/
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_EEPROM_Read(HANDLE hDevice,unsigned char *rbuf);
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_EEPROM_Write(HANDLE hDevice, unsigned char *wbuf);



/***************************** Counter and Pulse  Out *****************************/
//stop pulse out
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_PEnd(HANDLE hDevice,__int32 pch);

//start pout and set out eb
//pch =0,1
//pmode: 0 PWM mode / 1 single pulse mode
//PWM mode: pdata0 cycle / padat1 high wideth
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_PRun(HANDLE hDevice,__int32 pch, __int32 pmode,__int32 pdata0, __int32 pdata1);

//return out state, =1 out =1 / =0 out =0
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_PState(HANDLE hDevice,__int32 pch);

//set timer data
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_PSetData(HANDLE hDevice,__int32 pch,__int32 pdata0, __int32 pdata1);

//read data: cnt data cdata / timer data tdata
//return: <0 error / =0 cnt not over / =1 cnt over
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_CNT_Read(HANDLE hDevice,__int32 cntch, __int32 *cdata, __int32 *tdata);

//run or start cnt ch
//cdata: initla cnt data
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_CNT_Run(HANDLE hDevice, __int32 cntch, __int32 cdata);



/***************************** DA *****************************/
//set da data
//dch=0:da channel0 /1:da channel 1
//dadata=0-4095: 12bit DA data
//dgs: 0: 0-5v / 1 : 0-10V / 2: +/-5V
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_DA(HANDLE hDevice,__int32 dch, __int32 dg, __int32 dadata);

//initial da
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_DA_INI(HANDLE hDevice);



/***************************** DA calibrate *****************************/
extern "C" __int32 __declspec(dllexport)  __stdcall DACAL(HANDLE hDevice, __int32 dach, __int32 dag, __int32 *daz, __int32 *daf);

//calibrate DA
extern "C" __int32 __declspec(dllexport)  __stdcall DACAL_EEPROM_WR2CH(HANDLE hDevice, __int32 *caldata);
extern "C" __int32 __declspec(dllexport)  __stdcall DACAL_EEPROM_RD2CH(HANDLE hDevice, __int32 *cdata);



/***************************** not for  user *****************************/
//read a byte from hardware
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_inb(HANDLE hdevice,int adr);
extern "C" __int32 __declspec(dllimport)  __stdcall MP412_inw(HANDLE hdevice,int adr);

extern "C" __int32 __declspec(dllexport)  __stdcall MP412_outw(HANDLE hDevice1,__int32 adr,__int32 data);
extern "C" __int32 __declspec(dllexport)  __stdcall MP412_outb(HANDLE hDevice1,__int32 adr,__int32 data);

extern "C" __int32 __declspec(dllexport)  __stdcall MP412_EEPROM_S_Read(HANDLE hDevice,unsigned char *rbuf);
extern "C" __int32 __declspec(dllexport)  __stdcall MP412_EEPROM_S_Write(HANDLE hDevice, unsigned char *wbuf);


extern "C" __int32 __declspec(dllexport) __stdcall MP412_EEPROM_CRD(HANDLE hDevice,__int32 adr, __int32 len , unsigned char *rdata);
extern "C" __int32 __declspec(dllexport) __stdcall MP412_EEPROM_CWR(HANDLE hDevice,__int32 adr, __int32 len , unsigned char *wdata);

extern "C" __int32 __declspec(dllexport)  __stdcall MP412_sam(HANDLE hDevice, __int32 ch, __int32 gain, __int32 sidi,__int32 naver);

extern "C" __int32 __declspec(dllexport)  __stdcall MP412_RDCAL(HANDLE hDevice,__int32 *caldata);
extern "C" __int32 __declspec(dllexport)  __stdcall MP412_WRCAL(HANDLE hDevice,__int32 *caldata);

extern "C" __int32 __declspec(dllexport)  MP412_BulkRead(HANDLE hDevice,UCHAR *rData,__int32 rLength);

extern "C" __int32 __declspec(dllexport)  __stdcall MP412_FIFO_SETRD(HANDLE hdevice,__int32 rdl);

extern "C" __int32 __declspec(dllexport)  __stdcall MP412_DA_Data(HANDLE hDevice,__int32 dch, __int32 dg, __int32 dadata);

#endif // MP412_H
