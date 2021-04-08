cmake_minimum_required(VERSION 3.5)

project(qt-editor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# QtCreator supports the following variables for Android, which are identical to qmake Android variables.
# Check https://doc.qt.io/qt/deployment-android.html for more information.
# They need to be set before the find_package( ...) calls below.

#if(ANDROID)
#    set(ANDROID_PACKAGE_SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/android")
#    if (ANDROID_ABI STREQUAL "armeabi-v7a")
#        set(ANDROID_EXTRA_LIBS
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libcrypto.so
#            ${CMAKE_CURRENT_SOURCE_DIR}/path/to/libssl.so)
#    endif()
#endif()

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Multimedia LinguistTools REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Multimedia LinguistTools REQUIRED)

#set(TS_FILES qt-editor_en_GB.ts)

add_subdirectory(3rdParty/libmodplug)

include_directories(3rdParty/libmodplug/src)
include_directories(3rdParty/jsonxx)

add_library(jsonxx 3rdParty/jsonxx/jsonxx.cc)
target_include_directories(jsonxx PUBLIC
    $<BUILD_INTERFACE:
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/jsonxx)
export(TARGETS jsonxx FILE jsonxx.cmake)
set_property(TARGET jsonxx PROPERTY FOLDER "3rdParty")

set(PROJECT_SOURCES
        main.cpp
        aboutdialog.ui
        editormainwindow.ui
        editortab.ui
        editormainwindow.cpp
        editormainwindow.hpp
        editortab.cpp
        editortab.hpp
        firewidget.cpp
        firewidget.hpp
        aboutdialog.cpp
        aboutdialog.hpp
        modthread.cpp
        modthread.hpp
        model.cpp
        model.hpp
        resizeablearrowitem.cpp
        resizeablearrowitem.hpp
        resizeablerectitem.cpp
        resizeablerectitem.hpp
        resources.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(qt-editor
        ${PROJECT_SOURCES}
    )

    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
else()
    if(ANDROID)
        add_library(qt-editor SHARED
            ${PROJECT_SOURCES}
        )
    else()
        add_executable(qt-editor
            ${PROJECT_SOURCES}
        )
    endif()

    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
endif()

target_link_libraries(qt-editor PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia modplug jsonxx)
