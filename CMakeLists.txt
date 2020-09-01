cmake_minimum_required(VERSION 3.6)

set(CMAKE_CXX_STANDARD 11)

project(assignment1)
add_executable(assignment1 main.cpp)

set(CMAKE_OSX_SYSROOT /Library/Developer/CommandLineTools)
# glfw
find_package(glfw3 REQUIRED)

# opengl
find_package(OpenGL REQUIRED)
include_directories(${OpenGL_INCLUDE_DIRS})
link_directories(${OpenGL_LIBRARY_DIRS})
add_definitions(${OpenGL_DEFINITIONS})

# glew
find_package(GLEW REQUIRED)

include_directories(../nanogui/include ../nanogui/ext/eigen ../nanogui/ext/nanovg/src)
target_link_directories(assignment1 PUBLIC ../nanogui/build)

target_link_libraries(assignment1 ${OPENGL_LIBRARIES} GLEW::GLEW glfw nanogui)
