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

CONFIG += c++11

DEFINES *= QT_USE_QSTRINGBUILDER

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG QT_NO_DEBUG_OUTPUT

QMAKE_MACOSX_DEPLOYMENT_TARGET = 10.15

INCLUDEPATH += ../../lenlab ../../include

unix {
    LIBS += -L../../lenlab/gui -lgui
    LIBS += -L../../lenlab/model -lmodel
    LIBS += -L../../lenlab/protocol -lprotocol
    LIBS += -L../../lenlab/usb -lusb

    # libusb
    macx {
        LIBS += -L/usr/local/lib -lusb-1.0
        INCLUDEPATH += /usr/local/include/libusb-1.0
    } else {
        CONFIG += link_pkgconfig
        PKGCONFIG += libusb-1.0
    }
}

win32 {
    CONFIG(debug, debug|release) {
        LIBS += -L../../lenlab/gui/debug -lgui
        LIBS += -L../../lenlab/model/debug -lmodel
        LIBS += -L../../lenlab/protocol/debug -lprotocol
        LIBS += -L../../lenlab/usb/debug -lusb
    }

    CONFIG(release, debug|release) {
        LIBS += -L../../lenlab/gui/release -lgui
        LIBS += -L../../lenlab/model/release -lmodel
        LIBS += -L../../lenlab/protocol/release -lprotocol
        LIBS += -L../../lenlab/usb/release -lusb
    }

    # libusb
    INCLUDEPATH += $$PWD/libusb/include/libusb-1.0
    LIBS += -L$$PWD/libusb/MinGW64/dll/ -llibusb-1.0
}
