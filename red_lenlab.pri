
CONFIG += c++11

CONFIG += qwt

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG QT_NO_DEBUG_OUTPUT

INCLUDEPATH += ../../lenlab ../../include

unix {
    LIBS += -L../../lenlab/gui -lgui
    LIBS += -L../../lenlab/model -lmodel
    LIBS += -L../../lenlab/protocol -lprotocol
    LIBS += -L../../lenlab/usb -lusb

    # libusb
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
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

    # qwt
    # Qt Creator uses this variable to construct the PATH variable for the dynamic linker to run lenlab
    CONFIG(release, debug|release): LIBS += -Lc:/Qwt-6.1.4/lib/ -lqwt
    else:CONFIG(debug, debug|release): LIBS += -Lc:/Qwt-6.1.4/lib/ -lqwtd

    # libusb
    INCLUDEPATH += $$PWD/libusb/include/libusb-1.0
    LIBS += -L$$PWD/libusb/MinGW32/dll/ -llibusb-1.0
}
