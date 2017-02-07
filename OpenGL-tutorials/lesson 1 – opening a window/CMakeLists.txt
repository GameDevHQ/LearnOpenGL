cmake_minimum_required(VERSION 3.6)
project(lesson_1)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

file(GLOB SOURCE_FILES src/*.cpp)
include_directories(include/)
add_executable(lesson_1 ${SOURCE_FILES})
