# QT Documentation suggest 3.16
cmake_minimum_required(VERSION 3.16)

project(TerrainDemo VERSION 1.0.0 LANGUAGES CXX)

add_compile_options(-Wall -Wextra -Wpedantic -g)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets OpenGL OpenGLWidgets)

if (COMMAND qt_standard_project_setup)
    qt_standard_project_setup()
else()
    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTOUIC ON)
endif()

set(CMAKE_AUTORCC ON)

qt_add_executable(TerrainDemo WIN32 MACOSX_BUNDLE
    resources.qrc
    src/mainwindow.ui
    src/mainwindow.cpp src/mainwindow.h
    src/mainview.cpp src/mainview.h
    src/userinput.cpp
    src/model.cpp src/model.h
    src/utility.cpp
    src/vertex.h
    src/main.cpp
)

target_include_directories(TerrainDemo PRIVATE ${CMAKE_SOURCE_DIR}/src)

target_link_libraries(TerrainDemo PRIVATE
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::OpenGL
    Qt${QT_VERSION_MAJOR}::OpenGLWidgets
)

# This is used for interoperability, do not remove even on linux;
# On linux, result is an executable;
# On Windows, result is a Win32 executable, instead of console executable, command prompt window is not created;
# On MacOS, result is executable bundle;
set_target_properties(TerrainDemo PROPERTIES
    WIN32_EXECUTABLE ON
    MACOSX_BUNDLE ON
)
