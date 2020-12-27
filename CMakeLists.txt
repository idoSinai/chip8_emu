cmake_minimum_required(VERSION 3.15)

project(chip8)

set(CMAKE_CXX_STANDARD 17)

include_directories(../include)

set(SOURCE_FILES
    ../src/chip8.cpp
    ../src/main.cpp)

add_executable(emulator ${SOURCE_FILES})
