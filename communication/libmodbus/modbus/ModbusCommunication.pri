#This file is included in .pro

HEADERS += \
    communication/libmodbus/modbus/config.h \
    communication/libmodbus/modbus/modbus.h \
    communication/libmodbus/modbus/modbus-private.h \
    communication/libmodbus/modbus/modbus-rtu.h \
    communication/libmodbus/modbus/modbus-rtu-private.h \
    communication/libmodbus/modbus/modbus-tcp.h \
    communication/libmodbus/modbus/modbus-tcp-private.h \
    communication/libmodbus/modbus/modbus-version.h

SOURCES += \
    communication/libmodbus/modbus/modbus.c \
    communication/libmodbus/modbus/modbus-data.c \
    communication/libmodbus/modbus/modbus-rtu.c \
    communication/libmodbus/modbus/modbus-tcp.c

win32 {
LIBS += \
    #link "ws2_32.dll"(Windows Sockets), required by ModBus & tcp server
    -lws2_32
#to deal with "warning: #warning Please include winsock2.h before windows.h "
#1) add a macro about "WIN32_LEAN_AND_MEAN" as "https://msdn.microsoft.com/en-us/library/ms737629(VS.85).aspx" says
#2) do the flowwing:(modify qendian.h)
# #ifndef QT_HAS_BUILTIN
# # ifdef __has_builtin
# #  define QT_HAS_BUILTIN(x)     __has_builtin(x)
# # else
# #  define QT_HAS_BUILTIN(x)     0
# # endif
# #endif
# //#ifdef __has_builtin
# //#  define QT_HAS_BUILTIN(x)     __has_builtin(x)
# //#else
# //#  define QT_HAS_BUILTIN(x)     0
# //#endif
}

#LOG_ERR in ModbusQueue
#DEFINES += ENABLE_MODBUS_WRITE_READ_ERROR_DEBUG #LOG_ERR in ModbusQueue
