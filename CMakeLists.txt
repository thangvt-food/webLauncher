cmake_minimum_required(VERSION 3.16)
project(WebLauncher VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets)
find_package(QTermWidget5 REQUIRED)

add_executable(weblauncher
    src/main.cpp
    src/app/window.cpp
    src/app/window.h
    src/app/scanner.cpp
    src/app/scanner.h
    src/app/trayicon.cpp
    src/app/trayicon.h
    src/app/settingsdialog.cpp
    src/app/settingsdialog.h
    src/core/project.cpp
    src/core/project.h
    src/core/database.cpp
    src/core/database.h
    src/core/util.cpp
    src/core/util.h
)

target_include_directories(weblauncher PRIVATE
    ${CMAKE_CURRENT_SOURCE_DIR}/src
    /usr/include/qtermwidget5
    ${QTERMWIDGET5_INCLUDE_DIRS}
)

target_link_libraries(weblauncher PRIVATE
    Qt5::Widgets
    qtermwidget5
)

set_target_properties(weblauncher PROPERTIES
    AUTOMOC ON
    CXX_VISIBILITY_PRESET hidden
    VISIBILITY_INLINES_HIDDEN ON
)

install(TARGETS weblauncher RUNTIME DESTINATION bin)
