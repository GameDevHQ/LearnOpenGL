cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Release)

set(CMAKE_BINARY_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_EXE_LINKER_FLAGS -lglfw3)

find_package(OPENGL REQUIRED)
if (OPENGL_FOUND)
    include_directories(${OPENGL_INCLUDE_DIRS})
    link_libraries(${OPENGL_LIBRARIES})
endif()

find_package(GLEW REQUIRED)
if (GLEW_FOUND)
    include_directories(${GLEW_INCLUDE_DIRS})
    link_libraries(${GLEW_LIBRARIES})
endif()

SET(EXTRA_LIBS ${OPENGL_LIBRARIES} ${GLEW_LIBRARIES})

# Lesson 1 – Opening a window
add_subdirectory("lesson 1 – opening a window")

# Lesson 2 - The first triangle
add_subdirectory("lesson 2 – the first triangle")

# Lesson 3 – Matrices
add_subdirectory("lesson 3 – matrices")
