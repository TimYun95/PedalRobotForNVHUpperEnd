contains(DEFINES, DOF_3_PEDAL_API){

#in .pro file, dof_3_pedal_api.pri must been included before dof_2_pedal.pri
DEFINES += \
    DOF_2_PEDAL

HEADERS += \
    $$PWD/pedalrobotuiapi.h

SOURCES += \
    $$PWD/pedalrobotuiapi.cpp

TEMPLATE = lib

DEFINES += \
    PEDAL_ROBOT_API_DISABLE_CAN_DEVICE \
    PEDAL_ROBOT_API_DISABLE_MESSAGE_BOX
}
