#ifndef ADCREADER_H
#define ADCREADER_H

//#include <pthread.h>
//#include <windows.h>
#include <map>
#include <string>

//#include <QThread>

//#include "common/printf.h"
#include "common/message.h"
#include "device/ADConvertor/ac6603.h"

class adcReader
{
public:
    adcReader();

    // read data from device
    int ReadData();

    //int LoadParameter();    

    //check if button pressed, return true if button status changed
    //button pressed data is stored in "pressedButtons"
    bool CheckButton();

    //check if force is too large
    bool isForceTooLarge(double forceValue);

    //return the avg force value
    double GetForceValue();

    //return the avg force AD value
    unsigned short GetForceADValue();

    //return false if ad data is wrong(not start)
    bool isADValid();

    bool isButtonPressed(int buttonIndex);

    bool isForceButtonClicked();

private:
    void* hDevice = (void*)-1;

    const int thresHigh = 27000;
    const int thresLow  = 1000;
    const double thresForce = 8;
    const double forceADScale = 1.9758;

    int LastADData[32];

    int currentPosForForceValue = 0;
    bool hasEnoughData = false;
    const static int forceAvgLength = 3;
    double forceValueList[forceAvgLength];
    bool lastForceButton = false;

public:
    // ADConverter data for 32 channels
    int ADData[32];//0-65535的数字量

    // current pressed buttons
    unsigned char pressedButtons;

    // last pressed buttons (10ms ago)
    unsigned char lastPressedButtons = md_stop;

    // button with its channel
    enum ADConverterChannel{
        FORWARD_b = 10, BACK_b  = 15,
        UP_b      = 11, DOWN_b  = 12,
        LEFT_b    = 14, RIGHT_b = 13,
        FORWARD_f = 24, BACK_f  = 30,
        UP_f      = 28, DOWN_f  = 22,
        LEFT_f    = 26, RIGHT_f = 20,
        FORCE_CHANNEL = 18};

    // butoon with its label
    std::map<int, std::string> buttons = {
        {FORWARD_b , "FORWARD_b" },
        {BACK_b    , "BACK_b"    },
        {UP_b      , "UP_b"      },
        {DOWN_b    , "DOWN_b"    },
        {LEFT_b    , "LEFT_b"    },
        {RIGHT_b   , "RIGHT_b"   },
        {FORWARD_f , "FORWARD_f" },
        {BACK_f    , "BACK_f"    },
        {UP_f      , "UP_f"      },
        {DOWN_f    , "DOWN_f"    },
        {LEFT_f    , "LEFT_f"    },
        {RIGHT_f   , "RIGHT_f"   }
    };

    // 编码顺序：从低-->高：上下左右前后
    std::map<int, int> buttonsEncoding = {
        {UP_b      , md_zplus  },
        {DOWN_b    , md_zminus },
        {LEFT_b    , md_xminus },
        {RIGHT_b   , md_xplus  },
        {FORWARD_b , md_yplus  },
        {BACK_b    , md_yminus}
    };
};

#endif // ADCREADER_H
