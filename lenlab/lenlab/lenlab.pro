#-------------------------------------------------
#
# Project created by QtCreator 2017-11-14T12:37:51
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = lenlab
TEMPLATE = app

CONFIG += c++11

CONFIG += qwt

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
# Note: qwt uses deprecated APIs and lenlab will fail to build

SOURCES += \
    main.cpp \
    gui/mainwindow.cpp \
    gui/signalform.cpp \
    gui/loggerform.cpp \
    gui/oscilloscopeform.cpp \
    gui/frequencyform.cpp \
    model/lenlab.cpp \
    model/logger.cpp \
    model/oscilloscope.cpp \
    model/frequency.cpp \
    model/signal.cpp \
    model/component.cpp

HEADERS += \
    gui/mainwindow.h \
    gui/signalform.h \
    gui/loggerform.h \
    gui/oscilloscopeform.h \
    gui/frequencyform.h \
    model/lenlab.h \
    model/logger.h \
    model/oscilloscope.h \
    model/frequency.h \
    model/signal.h \
    model/component.h

FORMS += \
    gui/mainwindow.ui \
    gui/signalform.ui \
    gui/loggerform.ui \
    gui/oscilloscopeform.ui \
    gui/frequencyform.ui

unix {
    include( /usr/lib/qt/mkspecs/features/qwt.prf )
}

win32 {
    # Qt Creator uses this variable to construct the PATH variable for the dynamic linker to run lenlab
    CONFIG(release, debug|release): LIBS += -Lc:/Qwt-6.1.3/lib/ -lqwt
    else:CONFIG(debug, debug|release): LIBS += -Lc:/Qwt-6.1.3/lib/ -lqwtd
}
