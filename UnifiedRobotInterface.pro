#-------------------------------------------------
#
# Project created by QtCreator 2016-01-07T14:34:48
#
#-------------------------------------------------

QT += core gui xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET   = UnifiedRobotInterface
DEFINES += ROBOT_INTERFACE
TEMPLATE = app

SOURCES += \
    main.cpp\
    mainwindow.cpp \
    uri.cpp \
    teach.cpp \
    configuration.cpp \
    common/globalvariables.cpp \
    common/message.cpp \
    common/robotparams.cpp \
    common/sectioninfo.cpp \
    workerthread.cpp \
    mylogger/logger.cpp \
    communicationmediator.cpp \
    assistantfunc/fileassistantfunc.cpp \
    mymathlib/mymathlib.cpp \
    xml/configurationbase.cpp \
    template/singleton.cpp \
    unifiedrobot.cpp \
    util/timeutil.cpp \
    miscconfigurationparam.cpp \
    util/highprecisiontimer.cpp

HEADERS  += \
    mainwindow.h \
    uri.h \
    teach.h \
    configuration.h \
    common/printf.h \
    common/globalvariables.h \
    common/message.h \
    common/robotparams.h \
    common/sectioninfo.h \
    workerthread.h \
    mylogger/logger.h \
    communicationmediator.h \
    assistantfunc/fileassistantfunc.h \
    mymathlib/mymathlib.h \
    xml/configurationbase.h \
    template/singleton.h \
    unifiedrobot.h \
    util/timeutil.h \
    actioncallbacktype.h \
    miscconfigurationparam.h \
    util/highprecisiontimer.h

FORMS  += \
    uri.ui

include(settingwidget/SettingWidget.pri)
include(fileoperation/FileOperation.pri)

include(communication/libmodbus/modbus/ModbusCommunication.pri)
SOURCES += \
    communication/MsgQueue.cpp \
    communication/libmodbus/communicator.cpp \
    communication/libmodbus/EndianConverter.cpp \
    communication/libmodbus/modbusqueue.cpp \
    communication/msgstructure.cpp \
    communication/libmodbus/msgreginfo.cpp

HEADERS += \
    communication/MsgQueue.h \
    communication/libmodbus/communicator.h \
    communication/libmodbus/EndianConverter.h \
    communication/msgtypes.h \
    communication/libmodbus/modbusqueue.h \
    communication/msgstructure.h \
    communication/libmodbus/msgreginfo.h \
    communication/messagehandler.h

DEFINES += \
#    ENABLE_LOGIN_PASSWORD \ #URI::on_pushButton_exitSettings_clicked()

#MyWidget
include(mywidget/MyWidget.pri)

#log4qt
include(mylogger/Log4QTWrapper.pri)

#WebSocket
include(MyWebSocket/MyWebSocket.pri)

#RobotType
include(robottype/RobotTypeDefine.pri)
include(robottype/medicalultrasoundrobot/dof_4_medical.pri)
include(robottype/pedalshiftgearswerve/dof_6_pedal_shiftgear_swerve.pri)
include(robottype/pedalrobotapi/dof_3_pedal_api.pri)
include(robottype/pedalrobot/dof_2_pedal.pri)
include(robottype/scarapaintrobot/dof_5_scara_paint.pri)
include(robottype/igusflexibleshaft/dof_5_igus_flexible_shaft.pri)
include(robottype/xyzvesselpuncture/dof_6_xyz_vessel_puncture.pri)

#UnifiedRobotApiLibrary
include(UnifiedRobotApiLibrary/UnifiedRobotApiLibrary.pri)
