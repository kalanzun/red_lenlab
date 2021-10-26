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

QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_measurementtest.cpp

win32 {
    CONFIG(debug, debug|release) PRE_TARGETDEPS += ../../lenlab/protocol/debug/libprotocol.a
    CONFIG(release, debug|release) PRE_TARGETDEPS += ../../lenlab/protocol/release/libprotocol.a
}

unix {
    PRE_TARGETDEPS += ../../lenlab/protocol/libprotocol.a
}

include(../../red_lenlab.pri)
