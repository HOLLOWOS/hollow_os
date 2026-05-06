cmake_minimum_required(VERSION 3.16)
project(hollow-install)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(hollow-install
    src/main.cpp
    src/mainwindow.cpp
    src/orchestrator.cpp
)

target_link_libraries(hollow-install Qt6::Widgets)
