#This file is included in .pro

contains(DEFINES, DOF_2_PEDAL){

SOURCES += \
    robottype/pedalrobot/pedalrobotui.cpp

HEADERS += \
    robottype/pedalrobot/pedalrobotui.h

FORMS  +=  \
    robottype/pedalrobot/pedalrobotui.ui

DEFINES += \
    ENABLE_PEDAL_ROBOT_WIDGETS \
    ENABLE_USBCAN_DEVICE \
    ENABLE_MP412_DEVICE \
    DISABLE_STUDY_LIMIT \
    #ENABLE_DEVICE_ERROR_DEBUG \ #MP412&USBCAN
    #ENABLE_MP412_WARNINGBOX \
    #ENABLE_CHECK_STUDY_FILE_TYPE \#SettingWidgetPedalRobotStudy & SettingWidgetPedalRobotStudyWLTC
}

# USB CanAnalystII
contains(DEFINES, ENABLE_USBCAN_DEVICE){
SOURCES += \
    device/usbcan/cananalyst.cpp

HEADERS += \
    device/usbcan/ControlCAN.h \
    device/usbcan/cananalyst.h

LIBS += \
    -L$$PWD/../../device/usbcan -lControlCAN

QMAKE_LFLAGS  += \
    --enable-stdcall-fixup
}

# 双诺 MP412
contains(DEFINES, ENABLE_MP412_DEVICE){
SOURCES +=  \
    device/MP412/mp412device.cpp

HEADERS += \
    device/MP412/mp412.h \
    device/MP412/mp412device.h

LIBS += \
    -L$$PWD/../../device/MP412 -lMP412
}

HEADERS += \
    $$PWD/pedalrobotnvh.h \
    $$PWD/pedalrobotusbprocessor.h \
    $$PWD/pedalpretest.h

SOURCES += \
    $$PWD/pedalrobotnvh.cpp \
    $$PWD/pedalrobotusbprocessor.cpp \
    $$PWD/pedalpretest.cpp

include($$PWD/simpleChoice.pri)

