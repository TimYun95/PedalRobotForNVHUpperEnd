#include "adcReader.h"

#include <stdio.h>
#include <QMessageBox>

adcReader::adcReader()
{
    this->hDevice = AC6603_OpenDevice(0);//open
    if(hDevice == (void*)-1)
    {
        //("error open AC6603 Device!");        
        QMessageBox::critical(0, "Error" , "Could not open AC6603 Device!");
    }

    AC6603_CAL(hDevice);//AD矫正操作

    pressedButtons = 0;
    for (int i=0; i<forceAvgLength; i++)
        forceValueList[i] = 0;
}

bool adcReader::isADValid()
{
    int count = 0;
    for (int i=0; i<32; i++)
        if (ADData[i] < thresLow)
            count ++;

    if (count > 31) //有32个读数都是0，说明不大正常
        return false;
    else
        return true;
}

double adcReader::GetForceValue()//转换后的数值
{
    /*方法1*/
    double force = ADData[FORCE_CHANNEL] * 10 / 65535.0;
    force = 3.4262*force - 15.683; //forceADScale是力传感器的比例系数
    return force;

    /*方法2*/
    forceValueList[currentPosForForceValue] = force;
    currentPosForForceValue ++;
    if (currentPosForForceValue == forceAvgLength)
    {
        currentPosForForceValue = 0;
        hasEnoughData = true;
    }

    if (hasEnoughData)//测过去forceAvgLength个点的平均
    {
        double avgForceValue = 0;
        for (int i=0; i<forceAvgLength; i++)
            avgForceValue += forceValueList[i];
        avgForceValue = avgForceValue / forceAvgLength;

        return avgForceValue;
    }
    else
        return force;
}

unsigned short adcReader::GetForceADValue()//0-65535
{
    return ADData[FORCE_CHANNEL];
}

int adcReader::ReadData()
{
    for (int i = 0; i < 32; i++)
    {
        LastADData[i] = ADData[i];
        ADData[i] = AC6603_AD(hDevice,i,1);//读数据放入Data中
    }
    return 0;
}

bool adcReader::isForceButtonClicked()
{
    if ((ADData[UP_f] < thresLow) && (LastADData[UP_f] > thresHigh))
    {
        return true;
    }
    return false;
}

bool adcReader::CheckButton()
{
    std::map<int, std::string>::iterator itr;
    int buttonCount = 0;
    pressedButtons = 0;

    for (itr = buttons.begin(); itr != buttons.end(); itr++)
    {
        int index = itr->first;

        if (buttonsEncoding.find(index) != buttonsEncoding.end())
        {
            if (isButtonPressed(index))  // button[index] pressed continueally
            {
                pressedButtons = pressedButtons | buttonsEncoding[index];
                buttonCount ++;
            }
        }
    }

    if (buttonCount > 2) // too many button pressed!
    {
        pressedButtons = lastPressedButtons;
        return false;
    }

    if (pressedButtons != lastPressedButtons) // find button changed
    {
        lastPressedButtons = pressedButtons;
        return true;
    }

    return false;
}

bool adcReader::isButtonPressed(int buttonIndex)
{
    return ((ADData[buttonIndex] < thresLow) && (LastADData[buttonIndex] < thresLow));
}

bool adcReader::isForceTooLarge(double forceValue)
{
    return (forceValue < thresForce);
}

/*
int adcReader::ReadDataFromSharedMemory()
{
    const DWORD memoryFileSize = 4 * 32;
    HANDLE hMapFile;
    int* pBuf;
    const LPCTSTR memoryFileName = TEXT("ADConverterMappingObject");

    hMapFile = CreateFileMapping(
        INVALID_HANDLE_VALUE,    // use paging file
        NULL,                    // default security
        PAGE_READWRITE,          // read/write access
        0,                       // maximum object size (high-order DWORD)
        memoryFileSize,                // maximum object size (low-order DWORD)
        memoryFileName);                 // name of mapping object

    if (hMapFile == NULL)
    {
        PRINTF("Could not create file mapping object (%d).\n");
        return 1;
    }
    pBuf = (int *)MapViewOfFile(hMapFile,   // handle to map object
        FILE_MAP_ALL_ACCESS, // read/write permission
        0,
        0,
        memoryFileSize);

    if (pBuf == NULL)
    {
        PRINTF("Could not map view of file (%d).\n");
        CloseHandle(hMapFile);
        return 1;
    }
    while (1)
    {
        for (int i = 0; i < 32; i++)
        {
            int data = pBuf[i];
            ADData[i] = data;
        }
    }
}*/
