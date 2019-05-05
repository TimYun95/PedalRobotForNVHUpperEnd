#This file is included in .pro

SOURCES += \
    $$PWD/myinputdialog.cpp \
    $$PWD/qcustomplot.cpp \
    $$PWD/my2dgraph.cpp \
    $$PWD/mymessagebox.cpp \
    $$PWD/mywidgethelper.cpp

HEADERS  += \
    $$PWD/myinputdialog.h \
    $$PWD/qcustomplot.h \
    $$PWD/my2dgraph.h \
    $$PWD/mymessagebox.h \
    $$PWD/mywidgethelper.h
	
FORMS  += \
    $$PWD/myinputdialog.ui

QT += \
    printsupport #required by QCustomPlot(Get this class from Github)

#to deal with "warning: "QT_HAS_BUILTIN" redefined"
#edit "qendian.h" as "http://www.jsjtt.com/bianchengyuyan/cyuyan/46.html" says
