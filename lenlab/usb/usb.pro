#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T19:56:13
#
#-------------------------------------------------

QT       -= gui

TARGET = usb
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    active.cpp \
    activeguard.cpp \
    context.cpp \
    devhandle.cpp \
    device.cpp \
    devicelist.cpp \
    eventloop.cpp \
    exception.cpp \
    interface.cpp \
    notetoself.cpp \
    thread.cpp \
    transfer.cpp \
    bus.cpp \
    packet.cpp

HEADERS += \
    active.h \
    activeguard.h \
    context.h \
    devhandle.h \
    device.h \
    devicelist.h \
    eventloop.h \
    exception.h \
    interface.h \
    notetoself.h \
    thread.h \
    transfer.h \
    bus.h \
    packet.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../../red_lenlab.pri)
