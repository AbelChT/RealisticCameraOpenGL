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

add_executable(TestingOpenGL OpenglUtils.cpp OpenglUtils.h main.cpp SceneRenderer.h SceneDescription.h SceneRenderer.cpp base_code/obj.h base_code/obj.cc base_code/png.h base_code/png.cc SceneDescription.cpp)
# add_executable(TestingOpenGL base_code/main.cc base_code/world.h base_code/world.cc base_code/obj.h base_code/obj.cc base_code/png.h base_code/png.cc)
# add_executable(TestingOpenGL main_assimp_test.cpp SceneDescription.h)

target_link_libraries(TestingOpenGL OpenGL::GL OpenGL::GLU glfw glm assimp GLEW PNG::PNG)