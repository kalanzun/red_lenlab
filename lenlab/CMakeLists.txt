cmake_minimum_required(VERSION 3.5)

project(lenlab VERSION 0.1 LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets REQUIRED)

set(PROJECT_SOURCES
        main.cpp
        app/mainwindow.cpp
        app/mainwindow.h
        app/mainwindow.ui
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(lenlab
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
    )
else()
    add_executable(lenlab
        ${PROJECT_SOURCES}
    )
endif()

target_link_libraries(lenlab PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)

find_path(LENLAB_INCLUDE_DIR
    NAMES lenlab_protocol.h
    PATH_SUFFIXES "../include"
    REQUIRED
)

target_include_directories(lenlab PRIVATE ${LENLAB_INCLUDE_DIR})

find_path(LIBUSB_INCLUDE_DIR
    NAMES libusb.h
    PATH_SUFFIXES "../libusb/include/libusb-1.0"
    REQUIRED
)

target_include_directories(lenlab PRIVATE ${LIBUSB_INCLUDE_DIR})

find_library(LIBUSB_LIBRARY
    NAMES "libusb-1.0"
    PATH_SUFFIXES "../libusb/MinGW64/dll"
    REQUIRED
)

target_link_libraries(lenlab PRIVATE ${LIBUSB_LIBRARY})

set_target_properties(lenlab PROPERTIES
    MACOSX_BUNDLE_GUI_IDENTIFIER my.example.com
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(lenlab)
endif()
