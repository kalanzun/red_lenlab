#-------------------------------------------------
#
# Project created by QtCreator 2018-01-03T19:41:07
#
#-------------------------------------------------

QT       += widgets

TARGET = gui
TEMPLATE = lib
CONFIG += staticlib

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
#DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
# Note: qwt uses deprecated APIs and lenlab will fail to build

SOURCES += \
    frequencyform.cpp \
    loggerform.cpp \
    mainwindow.cpp \
    oscilloscopeform.cpp \
    pointvectorseriesdata.cpp \
    signalform.cpp \

HEADERS += \
    frequencyform.h \
    loggerform.h \
    mainwindow.h \
    oscilloscopeform.h \
    pointvectorseriesdata.h \
    signalform.h \

FORMS += \
    frequencyform.ui \
    loggerform.ui \
    mainwindow.ui \
    oscilloscopeform.ui \
    signalform.ui \

PRE_TARGETDEPS += ../model/debug/libmodel.a ../usb/debug/libusb.a

include(../../red_lenlab.pri)

unix {
    target.path = /usr/lib
    INSTALLS += target
}
