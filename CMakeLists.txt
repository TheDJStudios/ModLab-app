cmake_minimum_required(VERSION 4.2)

project(Velocity_App)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(CURL REQUIRED)

qt_standard_project_setup()

qt_add_executable(Velocity_App
        main.cpp
        extra/logger.h
)

target_link_libraries(Velocity_App PRIVATE
        Qt6::Widgets
        CURL::libcurl
)
