
CONFIG += c++11

CONFIG += qwt

CONFIG(release, debug|release): DEFINES += QT_NO_DEBUG QT_NO_DEBUG_OUTPUT

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

INCLUDEPATH += ../../lenlab ../../include

unix {
    LIBS += -L../../lenlab/gui -lgui
    LIBS += -L../../lenlab/model -lmodel
    LIBS += -L../../lenlab/usb -lusb

    # libusb
    CONFIG += link_pkgconfig
    PKGCONFIG += libusb-1.0
}

win32 {
    CONFIG(debug, debug|release) {
        #LIBS += -L../../lenlab/gui/debug -lgui
        #LIBS += -L../../lenlab/model/debug -lmodel
        LIBS += -L../../lenlab/protocol/debug -lprotocol
        LIBS += -L../../lenlab/usb/debug -lusb
    }

    CONFIG(release, debug|release) {
        #LIBS += -L../../lenlab/gui/release -lgui
        #LIBS += -L../../lenlab/model/release -lmodel
        LIBS += -L../../lenlab/protocol/release -lprotocol
        LIBS += -L../../lenlab/usb/release -lusb
    }

    # qwt
    # Qt Creator uses this variable to construct the PATH variable for the dynamic linker to run lenlab
    CONFIG(release, debug|release): LIBS += -Lc:/Qwt-6.1.3/lib/ -lqwt
    else:CONFIG(debug, debug|release): LIBS += -Lc:/Qwt-6.1.3/lib/ -lqwtd

    # libusb
    INCLUDEPATH += $$PWD/libusb/include/libusb-1.0
    LIBS += -L$$PWD/libusb/MinGW32/dll/ -llibusb-1.0
}
