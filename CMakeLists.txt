cmake_minimum_required(VERSION 3.16)
project(AtCoder-Contest-Helper LANGUAGES CXX)

include(qt.cmake)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)
find_package(Qt${QT_VERSION_MAJOR}
    COMPONENTS
        Core
        Gui
        Widgets
        Network
        PrintSupport
)
qt_standard_project_setup()

set(PROJECT_SOURCES
    main.cpp
    MainWindow.ui
    MainWindow.h
    MainWindow.cpp
    MainWindow.qrc
    qcustomplot.h
    qcustomplot.cpp
)

qt_add_executable(AtCoder-Contest-Helper ${PROJECT_SOURCES})

set_target_properties(AtCoder-Contest-Helper
    PROPERTIES
        WIN32_EXECUTABLE TRUE
)

target_link_libraries(AtCoder-Contest-Helper
    PUBLIC
        Qt::Core
        Qt::Gui
        Qt::Widgets
)
find_package(Qt6 REQUIRED COMPONENTS Network Charts) 
target_link_libraries(AtCoder-Contest-Helper PRIVATE Qt${QT_VERSION_MAJOR}::Widgets Qt6::Network Qt6::PrintSupport)