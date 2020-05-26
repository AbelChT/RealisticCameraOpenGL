cmake_minimum_required(VERSION 3.13)
project(TestingOpenGL)

set(CMAKE_CXX_STANDARD 20)

include(FetchContent)

# Find required packages to work with OpenGL
find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(GLEW REQUIRED)
find_package(PNG REQUIRED)

# Find packages to work with JSON
FetchContent_Declare(json
        GIT_REPOSITORY https://github.com/ArthurSonzogni/nlohmann_json_cmake_fetchcontent
        GIT_TAG v3.7.3)

FetchContent_GetProperties(json)
if (NOT json_POPULATED)
    FetchContent_Populate(json)
    add_subdirectory(${json_SOURCE_DIR} ${json_BINARY_DIR} EXCLUDE_FROM_ALL)
endif ()

# Configure project
add_executable(TestingOpenGL OpenglUtils.cpp OpenglUtils.h main.cpp SceneRenderer.h SceneDescription.h SceneRenderer.cpp libs/obj.h libs/obj.cc libs/png.h libs/png.cc SceneDescription.cpp SceneReader.cpp SceneReader.h)

target_link_libraries(TestingOpenGL OpenGL::GL OpenGL::GLU glfw glm GLEW PNG::PNG nlohmann_json::nlohmann_json)