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

set(TS_FILES qt-editor_en_GB.ts qt-editor_German.ts)

add_subdirectory(3rdParty/libmodplug)
add_subdirectory(3rdParty/alive_reversing/Source/Tools/relive_api)

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
        Source/SnapSettings.hpp
        Source/ClipBoard.cpp
        Source/ClipBoard.hpp
        Source/DeleteItemsCommand.cpp
        Source/DeleteItemsCommand.hpp
        Source/ProgressDialog.hpp
        Source/ProgressDialog.cpp
        Source/ProgressDialog.ui
        Source/TransparencyDialog.hpp
        Source/TransparencyDialog.cpp
        Source/TransparencyDialog.ui
        Source/SelectionSaver.hpp
        Source/AddObjectDialog.hpp
        Source/AddObjectDialog.cpp
        Source/AddObjectDialog.ui
        Source/ChangeMapSizeDialog.hpp
        Source/ChangeMapSizeDialog.cpp
        Source/ChangeMapSizeDialog.ui
        Source/PathDataEditorDialog.hpp
        Source/PathDataEditorDialog.cpp
        Source/PathDataEditorDialog.ui
        Source/MessageEditorDialog.cpp
        Source/MessageEditorDialog.hpp
        Source/MessageEditorDialog.ui
        Source/CameraManager.cpp
        Source/CameraManager.hpp
        Source/CameraManager.ui
        Source/ExportPathDialog.hpp
        Source/ExportPathDialog.cpp
        Source/Exporter.hpp
        Source/Exporter.cpp
        Source/ExportPathDialog.ui
        Source/PathSelectionDialog.hpp
        Source/PathSelectionDialog.cpp
        Source/PathSelectionDialog.ui
        Source/IGraphicsItem.hpp
        Source/main.cpp
        Source/AboutDialog.ui
        Source/EditorMainWindow.ui
        Source/EditorTab.ui
        Source/EditorMainWindow.cpp
        Source/EditorMainWindow.hpp
        Source/EditorTab.cpp
        Source/EditorTab.hpp
        Source/FireWidget.cpp
        Source/FireWidget.hpp
        Source/AboutDialog.cpp
        Source/AboutDialog.hpp
        Source/ModThread.cpp
        Source/ModThread.hpp
        Source/Model.cpp
        Source/Model.hpp
        Source/ResizeableArrowItem.cpp
        Source/ResizeableArrowItem.hpp
        Source/ResizeableRectItem.cpp
        Source/ResizeableRectItem.hpp
        Source/CameraGraphicsItem.cpp
        Source/CameraGraphicsItem.hpp
        Source/EditorGraphicsScene.cpp
        Source/EditorGraphicsScene.hpp
        Source/BigSpinBox.cpp
        Source/BigSpinBox.hpp
        Source/PropertyTreeItemBase.hpp
        Source/StringProperty.hpp
        Source/StringProperty.cpp
        Source/PropertyTreeWidget.cpp
        Source/PropertyTreeWidget.hpp
        Source/BasicTypeProperty.hpp
        Source/BasicTypeProperty.cpp
        Source/BasicTypeProperty.hpp
        Source/BasicTypeProperty.cpp
        Source/EnumProperty.cpp
        Source/EnumProperty.hpp
        Source/resources.qrc
        ${TS_FILES}
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    qt_add_executable(qt-editor ${PROJECT_SOURCES} ${QM_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_SOURCE_DIR} ${TS_FILES})
    if(ANDROID)
        add_library(qt-editor SHARED ${PROJECT_SOURCES} ${QM_FILES})
    else()
        add_executable(qt-editor ${PROJECT_SOURCES} ${QM_FILES})
    endif()
endif()

if (MSVC)
target_compile_options(qt-editor PUBLIC "/permissive-")
endif()

target_link_libraries(qt-editor PUBLIC relive_api Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia modplug jsonxx)
