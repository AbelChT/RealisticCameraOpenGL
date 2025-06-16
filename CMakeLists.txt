cmake_minimum_required(VERSION 3.13)
project(TestingOpenGL)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# Find required packages to work with OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(PNG CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)

# Configure project
add_executable(TestingOpenGL OpenglUtils.cpp 
                             OpenglUtils.h 
                             main.cpp
                             SceneRenderer.h
                             SceneDescription.h 
                             SceneRenderer.cpp 
                             libs/obj.h 
                             libs/obj.cc 
                             libs/png.h 
                             libs/png.cc 
                             SceneDescription.cpp 
                             SceneReader.cpp 
                             SceneReader.h)

target_link_libraries(TestingOpenGL PRIVATE OpenGL::GL OpenGL::GLU glfw glm::glm GLEW::GLEW PNG::PNG nlohmann_json::nlohmann_json)