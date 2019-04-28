#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T19:56:13
#
#-------------------------------------------------

QT       -= gui

TARGET = usb
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    active.cpp \
    activeguard.cpp \
    context.cpp \
    devhandle.cpp \
    device.cpp \
    devicelist.cpp \
    eventloop.cpp \
    interface.cpp \
    notetoself.cpp \
    thread.cpp \
    transfer.cpp \
    bus.cpp \
    packet.cpp \
    usberror.cpp

HEADERS += \
    active.h \
    activeguard.h \
    context.h \
    devhandle.h \
    device.h \
    devicelist.h \
    eventloop.h \
    interface.h \
    notetoself.h \
    thread.h \
    transfer.h \
    bus.h \
    packet.h \
    usberror.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../../red_lenlab.pri)
