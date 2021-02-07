QT += core serialport
QT += core network
QT -= gui

CONFIG += c++11

target.path = /app_qt5
INSTALLS += target

TARGET = get-sound
CONFIG += console
CONFIG -= app_bundle

TEMPLATE = app

SOURCES += main.cpp \
    tcpclient.cpp \
    sysvar.cpp \
    scpi/cmdscpi.cpp \
    scpi/sysscpi.cpp \
    getalsadata.cpp \
    uart.cpp \
    mainctrl.cpp \
    gpio_cntrl.cpp \
    uartmic.cpp


win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -lasound
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -lasound
else:unix: LIBS += -L$$PWD/lib/ -lasound

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

win32:CONFIG(release, debug|release): LIBS += -L$$PWD/lib/release/ -latopology
else:win32:CONFIG(debug, debug|release): LIBS += -L$$PWD/lib/debug/ -latopology
else:unix: LIBS += -L$$PWD/lib/ -latopology

INCLUDEPATH += $$PWD/include
DEPENDPATH += $$PWD/include

HEADERS += \
    tcpclient.h \
    sysvar.h \
    scpi/basescpi.h \
    scpi/command.h \
    scpi/sysscpi.h \
    common/sysstruct.h \
    getalsadata.h \
    uart.h \
    mainctrl.h \
    gpio_cntrl.h \
    uartmic.h


unix:!macx: LIBS += -L$$PWD/scpi/ -lbasescpi

INCLUDEPATH += $$PWD/scpi
DEPENDPATH += $$PWD/scpi

unix:!macx: PRE_TARGETDEPS += $$PWD/scpi/libbasescpi.a
