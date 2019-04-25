QT += testlib
QT -= gui

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

TEMPLATE = app

SOURCES +=  tst_protocoltest.cpp

# rebuild if those files have changed
PRE_TARGETDEPS = ../../lenlab/protocol/debug/libprotocol.a

include(../../red_lenlab.pri)
