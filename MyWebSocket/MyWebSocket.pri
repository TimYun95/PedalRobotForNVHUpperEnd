#This file is included in .pro

##################
# Notice: the websocket may not be ready for dof_4_medical and dof_2_pedal.
# The application of websocket communication is started from dof_5_scara_paint.
##################

# 1) used in CommunicationMediator
# This Macro controls whether the WebSocket communication is working. (Whether use the WebSocketFakeAdapter)
# 2) used in URI
# This Macro controls wheter the corresponding setting widget (SettingWidgetWebSocket) should be displayed.
#DEFINES += ENABLE_WEBSOCKET_COMMUNICATION

HEADERS += \
    $$PWD/websocketfakeadapter.h \
    $$PWD/websocket2modbus.h
SOURCES += \
    $$PWD/websocketfakeadapter.cpp \
    $$PWD/websocket2modbus.cpp

contains(DEFINES, ENABLE_WEBSOCKET_COMMUNICATION){

HEADERS += \
    $$PWD/mytypedefine.h \
    $$PWD/websocketadapter.h \
    $$PWD/websocketcommunicator.h \
    $$PWD/MySources/websocketclient.h \
    $$PWD/MySources/basicsemaphore.h \
    $$PWD/MySources/websocketmessage.h \
    $$PWD/MySources/websocketconfigurationmessage.h \
    $$PWD/MySources/websocketmsgtypes.h \
    $$PWD/MySources/websocketmsgqueue.h \
    $$PWD/MySources/websocketmsgnames.h \
    $$PWD/MySources/websocketcommonmsg.h

SOURCES += \
    $$PWD/websocketadapter.cpp \
    $$PWD/websocketcommunicator.cpp \
    $$PWD/MySources/websocketclient.cpp \
    $$PWD/MySources/basicsemaphore.cpp \
    $$PWD/MySources/websocketmessage.cpp \
    $$PWD/MySources/websocketconfigurationmessage.cpp \
    $$PWD/MySources/websocketmsgqueue.cpp \
    $$PWD/MySources/websocketcommonmsg.cpp

INCLUDEPATH += \
    $$PWD/ \ #websocketpp include file
    E:\boost_1_64_0_qt\bin\include\boost-1_64 #boost inculde file

LIBS += \
    -LE:\boost_1_64_0_qt\stage\lib #boost lib

LIBS += \
    -llibboost_system-mgw53-mt-1_64 \#the asio network lib for WebSocketServer
    -llibboost_thread-mgw53-mt-1_64 \#multi threads for WebSocketServer
    -lpthread -lws2_32 -lmswsock     #socket network for WebSocketServer

DEFINES += ENABLE_OPEN_SSL_LIB
include($$PWD/MySources/OpenSSLWrapper/OpenSSLWrapper.pri)#encryption algorithm
include($$PWD/MySources/JsonWrapper/JsonWrapper.pri)#Json String

#fix warning in Release mode
#boost warning: dereferencing type-punned pointer will break strict-aliasing
DEFINES += BOOST_THREAD_PROVIDES_GENERIC_SHARED_MUTEX_ON_WIN

}
