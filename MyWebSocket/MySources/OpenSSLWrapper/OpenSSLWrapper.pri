#This file is included in .pro

contains(DEFINES, ENABLE_OPEN_SSL_LIB){

HEADERS += \
    $$PWD/rsaencryptor.h \
    $$PWD/aesencryptor.h

SOURCES += \
    $$PWD/applink.c \
    $$PWD/rsaencryptor.cpp \
    $$PWD/aesencryptor.cpp

INCLUDEPATH += \
    E:\OpenSSL-Win32\include

LIBS += \
    -LE:\OpenSSL-Win32

LIBS += \
    -llibeay32 -llibssl32 -lssleay32

}
