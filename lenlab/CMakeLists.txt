cmake_minimum_required(VERSION 3.13)

project(lenlab VERSION 8.0 LANGUAGES CXX)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Charts Test REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts Test REQUIRED)

find_path(LENLAB_INCLUDE_DIR
    NAMES lenlab_protocol.h
    PATH_SUFFIXES "../include"
    REQUIRED
)

find_path(LIBUSB_INCLUDE_DIR
    NAMES libusb.h
    PATH_SUFFIXES "../libusb/include/libusb-1.0"
    REQUIRED
)

find_library(LIBUSB_LIBRARY
    NAMES "libusb-1.0"
    PATH_SUFFIXES "../libusb/MinGW64/dll"
    REQUIRED
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(usb STATIC)
else()
    add_library(usb STATIC)
endif()

target_include_directories(usb PUBLIC ${LIBUSB_INCLUDE_DIR})
target_link_libraries(usb PUBLIC ${LIBUSB_LIBRARY})

target_sources(usb PRIVATE
    usb/context.h usb/context.cpp
    usb/devicehandle.h usb/devicehandle.cpp
    usb/devicelist.h usb/devicelist.cpp
    usb/interface.h usb/interface.cpp
    usb/packet.h usb/packet.cpp
    usb/transfer.h usb/transfer.cpp
    usb/usbexception.h usb/usbexception.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(protocol STATIC)
else()
    add_library(protocol STATIC)
endif()

target_link_libraries(protocol PUBLIC usb)

target_include_directories(protocol PUBLIC ${LENLAB_INCLUDE_DIR})
target_link_libraries(protocol PUBLIC Qt${QT_VERSION_MAJOR}::Core)

target_sources(protocol PRIVATE
    protocol/board.h protocol/board.cpp
    protocol/device.h protocol/device.cpp
    protocol/eventloop.h protocol/eventloop.cpp
    protocol/message.h protocol/message.cpp
    protocol/querythread.h protocol/querythread.cpp
    protocol/usbdevice.h protocol/usbdevice.cpp
    protocol/usbthread.h protocol/usbthread.cpp
    protocol/virtualdevice.h protocol/virtualdevice.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(model STATIC)
else()
    add_library(model STATIC)
endif()

target_link_libraries(model PUBLIC protocol)

target_sources(model PRIVATE
    model/component.h model/component.cpp
    model/lenlab.h model/lenlab.cpp
    model/logger.h model/logger.cpp
    model/oscilloscope.h model/oscilloscope.cpp
    model/parameter.h model/parameter.cpp
    model/signalgenerator.h model/signalgenerator.cpp
    model/spectrumanalyzer.h model/spectrumanalyzer.cpp
    model/waveform.h model/waveform.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_library(app STATIC)
else()
    add_library(app STATIC)
endif()

target_link_libraries(app PUBLIC model)

target_link_libraries(app PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts)

target_sources(app PRIVATE
    app/mainwindow.h app/mainwindow.cpp app/mainwindow.ui
    app/labchart.h app/labchart.cpp app/labchart.ui
    app/loggerform.h app/loggerform.cpp app/loggerform.ui
    app/oscilloscopeform.h app/oscilloscopeform.cpp app/oscilloscopeform.ui
    app/signalform.h app/signalform.cpp app/signalform.ui
    app/spectrumform.h app/spectrumform.cpp app/spectrumform.ui
    app/vlabel.h app/vlabel.cpp
)


if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lenlab
        MANUAL_FINALIZATION
        main.cpp
    )
else()
    add_executable(lenlab
        main.cpp
    )
endif()

target_link_libraries(lenlab PUBLIC app)

set_target_properties(lenlab PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER red-lenlab.readthedocs.io
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lenlab)
endif()


add_executable(ProtocolTest tests/tst_protocoltest.cpp)
add_test(NAME ProtocolTest COMMAND ProtocolTest)

target_link_libraries(ProtocolTest PUBLIC protocol)
target_link_libraries(ProtocolTest PUBLIC Qt${QT_VERSION_MAJOR}::Test)


add_executable(ModelTest tests/tst_modeltest.cpp)
add_test(NAME ModelTest COMMAND ModelTest)

target_link_libraries(ModelTest PUBLIC model)
target_link_libraries(ModelTest PUBLIC Qt${QT_VERSION_MAJOR}::Test)
