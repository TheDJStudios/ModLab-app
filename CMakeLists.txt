cmake_minimum_required(VERSION 4.2)
project(Velocity_App)

set(CMAKE_CXX_STANDARD 20)

add_executable(Velocity_App main.cpp
        windowing/VelocityX.cpp
        windowing/VelocityX.h)
