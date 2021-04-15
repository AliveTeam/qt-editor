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
include_directories(Source)

add_library(jsonxx 3rdParty/jsonxx/jsonxx.cc)
target_include_directories(jsonxx PUBLIC
    $<BUILD_INTERFACE:
    ${CMAKE_CURRENT_SOURCE_DIR}/3rdParty/jsonxx)
export(TARGETS jsonxx FILE jsonxx.cmake)
set_property(TARGET jsonxx PROPERTY FOLDER "3rdParty")

set(PROJECT_SOURCES
        Source/main.cpp
        Source/aboutdialog.ui
        Source/editormainwindow.ui
        Source/editortab.ui
        Source/editormainwindow.cpp
        Source/editormainwindow.hpp
        Source/editortab.cpp
        Source/editortab.hpp
        Source/firewidget.cpp
        Source/firewidget.hpp
        Source/aboutdialog.cpp
        Source/aboutdialog.hpp
        Source/modthread.cpp
        Source/modthread.hpp
        Source/model.cpp
        Source/model.hpp
        Source/resizeablearrowitem.cpp
        Source/resizeablearrowitem.hpp
        Source/resizeablerectitem.cpp
        Source/resizeablerectitem.hpp
        Source/CameraGraphicsItem.cpp
        Source/CameraGraphicsItem.hpp
        Source/EditorGraphicsScene.cpp
        Source/EditorGraphicsScene.hpp
        Source/bigspinbox.cpp
        Source/bigspinbox.hpp
        Source/PropertyTreeItemBase.hpp
        Source/StringProperty.hpp
        Source/StringProperty.cpp
        Source/PropertyTreeWidget.cpp
        Source/PropertyTreeWidget.hpp
        Source/resources.qrc
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
