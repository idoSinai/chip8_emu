cmake_minimum_required(VERSION 3.15)

project(chip8)

set(CMAKE_CXX_STANDARD 17)
set(SFML_LIBS sfml-system sfml-graphics sfml-window)

include_directories(../include)

set(SOURCE_FILES
    ../src/graphics.cpp
    ../src/chip8.cpp
    ../src/main.cpp)

add_executable(emulator ${SOURCE_FILES})
target_link_libraries(emulator ${SFML_LIBS})

