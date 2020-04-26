cmake_minimum_required(VERSION 3.13)
project(TestingOpenGL)

set(CMAKE_CXX_STANDARD 20)

# Find required packages to work with OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)

# Find required packages to load Assets
find_package(assimp REQUIRED)

# add_executable(TestingOpenGL main.cc world.h world.cc obj.h obj.cc png.h png.cc main_assimp_test.cpp)
add_executable(TestingOpenGL main_assimp_test.cpp SceneDescription.h)

target_link_libraries(TestingOpenGL OpenGL::GL OpenGL::GLU glfw glm assimp GLEW PNG::PNG)