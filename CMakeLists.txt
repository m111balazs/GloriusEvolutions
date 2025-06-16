cmake_minimum_required(VERSION 3.10)
project(GloriousEvolutions)

set(CMAKE_CXX_STANDARD 17)

# Add glad and glfw
add_subdirectory(lib/glfw)

# Include directories
include_directories(
    lib/glad/include
    lib/glfw/include
    lib/glm
    lib/imgui
    lib/imgui/backends
)

# Find OpenGL
find_package(OpenGL REQUIRED)

# Define all source files
set(SOURCES
    src/main.cpp
    lib/glad/src/glad.c
    lib/imgui/imgui.cpp
    lib/imgui/imgui_demo.cpp
    lib/imgui/imgui_draw.cpp
    lib/imgui/imgui_tables.cpp
    lib/imgui/imgui_widgets.cpp
    lib/imgui/backends/imgui_impl_opengl3.cpp
    lib/imgui/backends/imgui_impl_glfw.cpp
)

# Create single executable with all sources
add_executable(GloriousEvolutions ${SOURCES})

# Link libraries
target_link_libraries(GloriousEvolutions glfw ${OPENGL_gl_LIBRARY})