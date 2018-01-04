
CONFIG += c++11

CONFIG += qwt

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG QT_NO_DEBUG_OUTPUT

INCLUDEPATH += ../../lenlab ../../include

debug {
    LIBS += -L../../lenlab/gui/debug -lgui
    LIBS += -L../../lenlab/model/debug -lmodel
    LIBS += -L../../lenlab/usb/debug -lusb
}

release {
    LIBS += -L../../lenlab/gui/release -lgui
    LIBS += -L../../lenlab/model/release -lmodel
    LIBS += -L../../lenlab/usb/release -lusb
}

unix {
    # libusb
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

win32 {
    # qwt
    # Qt Creator uses this variable to construct the PATH variable for the dynamic linker to run lenlab
    CONFIG(release, debug|release): LIBS += -Lc:/Qwt-6.1.3/lib/ -lqwt
    else:CONFIG(debug, debug|release): LIBS += -Lc:/Qwt-6.1.3/lib/ -lqwtd

    # libusb
    INCLUDEPATH += $$PWD/libusb/include/libusb-1.0
    LIBS += -L$$PWD/libusb/MinGW32/dll/ -llibusb-1.0
}
