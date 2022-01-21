cmake_minimum_required(VERSION 3.16)

# C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

## Project Name ##
project(FreeRTOS-Cpp)

add_library(FreeRTOS-Cpp INTERFACE)

## Dependencies ##
target_link_libraries(FreeRTOS-Cpp INTERFACE
    FreeRTOS-Kernel
)

## Headers ##
target_include_directories(FreeRTOS-Cpp INTERFACE
    "${CMAKE_CURRENT_SOURCE_DIR}/include"
)
