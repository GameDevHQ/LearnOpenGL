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

# Lesson 4 – A colored cube
add_subdirectory("lesson 4 – a colored cube")

# Lesson 5 – A textured cube
add_subdirectory("lesson 5 – a textured cube")

# Lesson 6 – Keyboard and mouse
add_subdirectory("lesson 6 – keyboard and mouse")

# Lesson 7 – Model loading
add_subdirectory("lesson 7 – model loading")

# Lesson 8 – Basic shading
add_subdirectory("lesson 8 – basic shading")

# Lesson 9 – VBO indexing
add_subdirectory("lesson 9 – vbo indexing")

# Lesson 10 - Transparency
add_subdirectory("lesson 10 – transparency")

# Lesson 11 – 2D text
add_subdirectory("lesson 11 – 2d text")

# Lesson 12 – Extensions
add_subdirectory("lesson 12 – extensions")

# Lesson 13 – Normal mapping
add_subdirectory("lesson 13 – normal mapping")

# Lesson 14 - Render to texture
add_subdirectory("lesson 14 – render to texture")
