#-------------------------------------------------
#
# Project created by QtCreator 2018-11-17T13:10:19
#
#-------------------------------------------------

QT       -= gui

TARGET = protocol
TEMPLATE = lib
CONFIG += staticlib

SOURCES += \
    message.cpp \
    transaction.cpp \
    manager.cpp

HEADERS += \
    message.h \
    transaction.h \
    manager.h

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../../red_lenlab.pri)
