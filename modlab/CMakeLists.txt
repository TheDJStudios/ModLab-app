cmake_minimum_required(VERSION 3.20)
project(ModLab VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Widgets)

set(SOURCES
    src/main.cpp
    src/ui/MainWindow.cpp
    src/ui/Sidebar.cpp
    src/ui/PackCard.cpp
    src/ui/PackGrid.cpp
    src/ui/DetailPanel.cpp
    src/ui/NewPackDialog.cpp
    src/ui/TopBar.cpp
    src/core/PackManager.cpp
    src/ipc/BackendProcess.cpp
)

set(HEADERS
    src/ui/MainWindow.h
    src/ui/Sidebar.h
    src/ui/PackCard.h
    src/ui/PackGrid.h
    src/ui/DetailPanel.h
    src/ui/NewPackDialog.h
    src/ui/TopBar.h
    src/core/PackManager.h
    src/core/Pack.h
    src/ipc/BackendProcess.h
)

qt_add_executable(ModLab ${SOURCES} ${HEADERS})

target_link_libraries(ModLab PRIVATE Qt6::Core Qt6::Widgets)
target_include_directories(ModLab PRIVATE src)

# Point to the Python backend — adjust path at build time if needed
target_compile_definitions(ModLab PRIVATE
    MODLAB_BACKEND_PATH="${CMAKE_SOURCE_DIR}/../main.py"
)
