#
QWT_ROOT = C:\qwt-6.1.2-MinGw
QT += widgets serialport core gui

TARGET = Aggralinx
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    console.cpp \
    parser.cpp \
    datapacket.cpp \
    splash.cpp \
    dataplot.cpp \
    moisturedialog.cpp \
    instrumentdata.cpp \
    regression.cpp

HEADERS += \
    mainwindow.h \
    console.h \
    parser.h \
    datapacket.h \
    splash.h \
    dataplot.h \
    moisturedialog.h \
    instrumentdata.h \
    regression.h

FORMS += \
    mainwindow.ui \
    splash.ui \
    moisturedialog.ui

RESOURCES += \
    terminal.qrc

DISTFILES +=

win32{
RC_ICONS += ../icon/NDT_agg1.ico
VERSION = 3.1.0.0
QMAKE_TARGET_COMPANY = James Instruments Inc.
QMAKE_TARGET_PRODUCT = Aggralinx
QMAKE_TARGET_DESCRIPTION = Upload Software for James Instruments Aggrameter
QMAKE_TARGET_COPYRIGHT = @2016 James Instruments Inc.
}

win32:CONFIG(release, debug|release): LIBS += -L$${QWT_ROOT}/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$${QWT_ROOT}/lib/ -lqwtd

INCLUDEPATH += $${QWT_ROOT}/src
DEPENDPATH  += $${QWT_ROOT}/src
