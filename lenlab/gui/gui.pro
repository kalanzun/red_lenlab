#  Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
#  Copyright (C) 2017-2021 Christoph Simon and the Lenlab developer team
#  
#  This program is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with this program.  If not, see <https://www.gnu.org/licenses/>.

QT       += widgets charts printsupport

TARGET = gui
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
    frequencyform.cpp \
    labchart.cpp \
    loggerform.cpp \
    mainwindow.cpp \
    oscilloscopeform.cpp \
    signalform.cpp \
    verticallabel.cpp

HEADERS += \
    frequencyform.h \
    labchart.h \
    loggerform.h \
    mainwindow.h \
    oscilloscopeform.h \
    signalform.h \
    verticallabel.h

FORMS += \
    frequencyform.ui \
    labchart.ui \
    loggerform.ui \
    mainwindow.ui \
    oscilloscopeform.ui \
    signalform.ui

win32 {
    CONFIG(debug, debug|release) PRE_TARGETDEPS += ../model/debug/libmodel.a
    CONFIG(release, debug|release) PRE_TARGETDEPS += ../model/release/libmodel.a
}

unix {
    PRE_TARGETDEPS += ../model/libmodel.a
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../../red_lenlab.pri)
