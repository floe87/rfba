cmake_minimum_required(VERSION 3.16)

project(OMM_Parser VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(OMM_Parser main.cpp)

file(GLOB SOURCES "*.cpp" "*.h")
target_sources(OMM_Parser PRIVATE ${SOURCES})
