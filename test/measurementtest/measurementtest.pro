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
