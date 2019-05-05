#This file is included in .pro

#This robot is compiled as the dll
#  first, switch the "TEMPLATE" into "lib"
#  second, remember to compile it in "Release" mode

contains(DEFINES, DOF_4_MEDICAL){
TEMPLATE = lib #for .dll

SOURCES += \
    robottype/medicalultrasoundrobot/medicalultrasoundrobot.cpp \
    robottype/medicalultrasoundrobot/robotapi.cpp

HEADERS += \
    robottype/medicalultrasoundrobot/medicalultrasoundrobot.h \
    robottype/medicalultrasoundrobot/robotapi.h

DEFINES += \
    ENABLE_AC6603_DEVICE \
    ENABLE_FORCE_CONTROL_WIDGET
}

#required by AC6603 AD convertor#
contains(DEFINES, ENABLE_AC6603_DEVICE){
SOURCES += \
    device/ADConvertor/adcreader.cpp

HEADERS += \
    device/ADConvertor/ac6603.h \
    device/ADConvertor/adcReader.h

LIBS += \
    -L$$PWD/../../device/ADConvertor -lAC6603
}
