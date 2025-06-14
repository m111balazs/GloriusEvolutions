cmake_minimum_required(VERSION 3.10)
project(GloriousEvolutions)

set(CMAKE_CXX_STANDARD 17)

# Add glad and glfw
add_subdirectory(lib/glfw)

include_directories(
    lib/glad/include
    lib/glfw/include
    lib/glm
)

# Add glad source file
set(SOURCES
    src/main.cpp
    lib/glad/src/glad.c
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Main executable
add_executable(GloriousEvolutions ${SOURCES})

# Link libraries
target_link_libraries(GloriousEvolutions glfw ${OPENGL_gl_LIBRARY})