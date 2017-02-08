cmake_minimum_required(VERSION 3.6)
project(lesson_3)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SOURCE_FILES src/*.cpp)
include_directories(include/)

include_directories(../common)
file(GLOB COMMON_SOURCE_FILES ../common/*.cpp)
add_executable(lesson_3 ${SOURCE_FILES} ${COMMON_SOURCE_FILES})
