#Created for Localization of Aggrameter
#
#
QT += widgets serialport

TARGET = Aggralinx
TEMPLATE = app

SOURCES += \
    main.cpp \
    mainwindow.cpp \
    console.cpp \
    splash.cpp \
    dataplot.cpp \
    moisturedialog.cpp \
    instrumentdata.cpp \
    regression.cpp

HEADERS += \
    mainwindow.h \
    console.h \
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

TRANSLATIONS += Internationalization_de.ts \
               Internationalization_es.ts

win32{
RC_ICONS += ../icon/NDT_agg1.ico
VERSION = 1.0.0.0
QMAKE_TARGET_COMPANY = James Instruments Inc.
QMAKE_TARGET_PRODUCT = Aggralinx
QMAKE_TARGET_DESCRIPTION = Upload Software for James Instruments Aggrameter
QMAKE_TARGET_COPYRIGHT = @2016 James Instruments Inc.
}

#win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../qwt-6.1.2-MinGw/lib/ -lqwt
#else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../qwt-6.1.2/qwt-6.1.2-MinGw/lib/ -lqwtd

#win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../qwt-6.1.2-MinGw/lib/libqwt.a
#else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../qwt-6.1.2-MinGw/lib/libqwtd.a
#else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../qwt-6.1.2-MinGw/lib/qwt.lib
#else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../qwt-6.1.2-MinGw/lib/qwtd.lib

unix:!macx: LIBS += -L$$PWD/../../../qwt-6.1/lib/ -lqwt

#INCLUDEPATH += $$PWD/../../../qwt-6.1.2-MinGw/src
#DEPENDPATH += $$PWD/../../../qwt-6.1.2-MinGw/src

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/../../../../../../qwt-6.1.2-MinGw/lib/ -lqwt
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/../../../../../../qwt-6.1.2-MinGw/lib/ -lqwtd

INCLUDEPATH += $$PWD/../../../../../../qwt-6.1.2-MinGw/include
DEPENDPATH += $$PWD/../../../../../../qwt-6.1.2-MinGw/include

win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../qwt-6.1.2-MinGw/lib/libqwt.a
else:win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../qwt-6.1.2-MinGw/lib/libqwtd.a
else:win32:!win32-g++:CONFIG(release, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../qwt-6.1.2-MinGw/lib/qwt.lib
else:win32:!win32-g++:CONFIG(debug, debug|release): PRE_TARGETDEPS += $$PWD/../../../../../../qwt-6.1.2-MinGw/lib/qwtd.lib
