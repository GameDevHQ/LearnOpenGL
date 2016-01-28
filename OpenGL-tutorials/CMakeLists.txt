cmake_minimum_required(VERSION 3.3)

project(OpenGL-tutorials)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -I/usr/local/include -L/usr/local/lib -std=c++11")
set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXE_LINKER_FLAGS -lglfw3)

find_package(OPENGL REQUIRED)
include_directories(${OPENGL_INCLUDE_DIRS})

find_package(GLEW REQUIRED)
include_directories(${GLEW_INCLUDE_DIRS})

SET(EXTRA_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

# Lesson 1 – Opening a window
set(SOURCE_FILES "lesson 1 – opening a window/main.cpp")
add_executable(lesson_1 ${SOURCE_FILES})

# Lesson 1 – The first triangle
set(SOURCE_FILES "lesson 2 – the first triangle/main.cpp")
add_executable(lesson_2 ${SOURCE_FILES})
target_link_libraries(lesson_2 ${EXTRA_LIBS})
