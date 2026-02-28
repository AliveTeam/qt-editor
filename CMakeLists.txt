cmake_minimum_required(VERSION 3.5)

project(qt-editor LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# without this the editor will crash on startup:
# qt-editor: symbol lookup error: qt-editor: undefined symbol: _ZdlPvm, version Qt_5
if(UNIX)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-sized-deallocation")
endif()

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
add_subdirectory(assets)

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
        Source/ReliveApiWrapper.hpp
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
        Source/CollisionConnect.cpp
        Source/CollisionConnect.hpp
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
    qt_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source ${TS_FILES})
    qt_add_executable(qt-editor ${PROJECT_SOURCES} ${QM_FILES})
else()
    qt5_create_translation(QM_FILES ${CMAKE_CURRENT_SOURCE_DIR}/Source ${TS_FILES})
    if(ANDROID)
        add_library(qt-editor SHARED ${PROJECT_SOURCES} ${QM_FILES})
    elseif(WIN32)
        set(APP_ICON_RESOURCE_WINDOWS "${CMAKE_CURRENT_SOURCE_DIR}/Source/rsc/icons/qt-editor.rc")
        add_executable(qt-editor ${PROJECT_SOURCES} ${QM_FILES} ${APP_ICON_RESOURCE_WINDOWS})
    else()
        add_executable(qt-editor ${PROJECT_SOURCES} ${QM_FILES})
    endif()
endif()

if (MSVC)
target_compile_options(qt-editor PUBLIC "/permissive-")
endif()

target_link_libraries(qt-editor PUBLIC relive_api Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Multimedia modplug jsonxx)

set(CPACK_PACKAGE_NAME "relive-qt-editor")
set(CPACK_PACKAGE_CONTACT "nemin@oddwords.hu")
set(CPACK_DEBIAN_PACKAGE_HOMEPAGE "https://aliveteam.github.io/")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libqt5core5a, libqt5gui5, libqt5widgets5, libqt5network5, libqt5multimedia5")
set(CPACK_DEBIAN_PACKAGE_DESCRIPTION "An Open-Source Level Editor for Abe's Oddysee and Abe's Exoddus.
Team R.E.L.I.V.E. proudly presents the replacement for the legacy Paul's Level Editor.
 .
Compared to the old editor it allows for editing far more kinds of objects, removes the arbitrary limitations the original engine imposed, and features a vastly improved user interface.")

if(UNIX)
    set_target_properties(qt-editor PROPERTIES OUTPUT_NAME relive-qt-editor)
    install(TARGETS qt-editor DESTINATION bin)
endif()

INCLUDE(CPack)