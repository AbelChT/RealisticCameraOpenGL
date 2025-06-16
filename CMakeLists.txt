cmake_minimum_required(VERSION 3.13)
project(RealisticCameraOpenGL)

set(CMAKE_CXX_STANDARD 20)

# Find required packages to work with OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 CONFIG REQUIRED)
find_package(glm CONFIG REQUIRED)
find_package(GLEW CONFIG REQUIRED)
find_package(PNG CONFIG REQUIRED)
find_package(nlohmann_json CONFIG REQUIRED)
find_path(PNGPP_INCLUDE_DIRS "png++/color.hpp")


# Configure project
add_executable(${PROJECT_NAME} OpenglUtils.cpp 
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
target_include_directories(${PROJECT_NAME} PRIVATE ${PNGPP_INCLUDE_DIRS})
target_link_libraries(${PROJECT_NAME} PRIVATE OpenGL::GL OpenGL::GLU glfw glm::glm GLEW::GLEW PNG::PNG nlohmann_json::nlohmann_json)