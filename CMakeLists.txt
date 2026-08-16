#SS26 Raumfahrtbetrieb Übung Projekt CMake List

cmake_minimum_required(VERSION 3.16)

project(rfba LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(ueb1/OMM_Parser_Template)
add_subdirectory(ueb2/SGP4_Code)
add_subdirectory(ueb3/uebung3_code)