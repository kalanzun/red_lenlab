#  Lenlab, an oscilloscope software for the TI LaunchPad EK-TM4C123GXL
#  Copyright (C) 2017-2019 Christoph Simon and the Lenlab developer team
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

QT       -= gui

TARGET = model
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
    component.cpp \
    frequencysweep.cpp \
    lenlab.cpp \
    oscilloscope.cpp \
    signalgenerator.cpp \
    voltmeter.cpp \
    waveform.cpp \
    series.cpp \
    frequencyseries.cpp \
    indexparameter.cpp \
    utils.cpp \
    loggerseries.cpp

HEADERS += \
    component.h \
    frequencysweep.h \
    lenlab.h \
    oscilloscope.h \
    signalgenerator.h \
    voltmeter.h \
    waveform.h \
    series.h \
    frequencyseries.h \
    indexparameter.h \
    utils.h \
    loggerseries.h

win32 {
    CONFIG(debug, debug|release) PRE_TARGETDEPS += ../protocol/debug/libprotocol.a
    CONFIG(release, debug|release) PRE_TARGETDEPS += ../protocol/release/libprotocol.a
}

unix {
    PRE_TARGETDEPS += ../protocol/libprotocol.a
}

unix {
    target.path = /usr/lib
    INSTALLS += target
}

include(../../red_lenlab.pri)
