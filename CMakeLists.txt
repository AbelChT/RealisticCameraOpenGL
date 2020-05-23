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

# Find required packages to load Assets
find_package(assimp REQUIRED)

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
# add_executable(TestingOpenGL OpenglUtils.cpp OpenglUtils.h main.cpp SceneRenderer.h SceneDescription.h SceneRenderer.cpp libs/obj.h libs/obj.cc libs/png.h libs/png.cc SceneDescription.cpp SceneReader.cpp SceneReader.h main_tester.cpp)
add_executable(TestingOpenGL SceneDescription.cpp SceneReader.cpp SceneReader.h main_tester.cpp libs/obj.h libs/obj.cc libs/png.h libs/png.cc)
# add_executable(TestingOpenGL base_code/main.cc base_code/world.h base_code/world.cc base_code/obj.h base_code/obj.cc base_code/png.h base_code/png.cc)
# add_executable(TestingOpenGL main_assimp_test.cpp SceneDescription.h)

target_link_libraries(TestingOpenGL OpenGL::GL OpenGL::GLU glfw glm assimp GLEW PNG::PNG nlohmann_json::nlohmann_json)