cmake_minimum_required(VERSION 3.26)
project(phys-lab-0)
find_package(OpenGL REQUIRED)

set(CMAKE_CXX_STANDARD 17)

add_subdirectory(lib)
add_executable(phys-lab-0 main.cpp
        window.h
        window.cpp)
target_link_libraries(phys-lab-0 PUBLIC imgui imgui-glfw imgui-opengl3 OpenGL glfw)