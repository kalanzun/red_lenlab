QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_usbtest.cpp

win32 {
    CONFIG(debug, debug|release) PRE_TARGETDEPS += ../../lenlab/usb/debug/libusb.a
    CONFIG(release, debug|release) PRE_TARGETDEPS += ../../lenlab/usb/release/libusb.a
}

unix {
    PRE_TARGETDEPS += ../../lenlab/usb/libusb.a
}

include(../../red_lenlab.pri)
