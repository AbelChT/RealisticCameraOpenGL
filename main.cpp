//
// Created by abel on 27/4/20.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include "SceneDescription.h"
#include "SceneRenderer.h"
#include "base_code/obj.h"

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>


void initEngine() {
    // Meshes definition
    glm::mat4 xf = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    OBJ obj;
    obj.load("assets/teapot.obj", xf);

    std::cout << "Faces " << obj.faces().size() << std::endl;

    vector<SceneMesh> sceneMeshes{
            SceneMesh(obj.faces(), obj.normals(), obj.texcoord())
    };

    // Textures definition
    auto texture = PNG("tex/checker.png");
//    vector<PNG> textures{
//            PNG("tex/checker.png")
//    };

    // Light definition
    SceneLight sceneLight(glm::normalize(glm::vec3(1.0f)));

    // Camera definition
    glm::vec3 to(0, 0, 0);

    float world_ph = 0.0;
    float world_th = 30.0;

    float world_ro = 1.0;

    const float ph = glm::radians(world_ph);
    const float th = glm::radians(world_th);

    glm::vec3 axis(cos(ph) * cos(th), sin(ph) * cos(th), sin(th));
    float cameraDistanceFromO = 4.0f;
    glm::vec3 eye = to + cameraDistanceFromO * axis;
    SceneCamera sceneCamera(eye, to, 45.0f, 0.01f, 1000.0f);

    // Objects definition
    vector<ObjectDescription> sceneObjects{
            ObjectDescription(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1), glm::vec3(255, 30, 30),
                              false, true, 0),
            ObjectDescription(glm::vec3(-8, 2, 0), glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1), glm::vec3(30, 255, 30),
                              false, true, 0),
            ObjectDescription(glm::vec3(-32, -16, 0), glm::vec3(0, 0, 0),
                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
                              false, true, 0)
    };

    // Scene description
    SceneDescription sceneDescription(sceneMeshes, texture, sceneObjects, sceneLight, sceneCamera);

    // Init engine
    initSceneRenderer(sceneDescription);
}

void callbackOpenGLReshape(GLFWwindow *win, int w, int h) {
    glfwMakeContextCurrent(win);
    std::cout << "Callback: Refresh" << std::endl;
    reshapeScene(w, h);
}

bool style = false;

void callbackOpenGLDisplay(GLFWwindow *win) {
    std::cout << "Callback: Display" << std::endl;
    int w, h;
    glfwGetWindowSize(win, &w, &h);

    glfwMakeContextCurrent(win);

    if (!style)
        renderFrame(w, h, false);
    else
        renderFrame(w, h, true);

    glfwSwapBuffers(win);
}

void callbackOpenGLKeyboard(GLFWwindow *win, int key, int s, int act, int mod) {
    if (act == GLFW_RELEASE)
        return;

    switch (key) {
        case GLFW_KEY_R:
            style = !style;
            callbackOpenGLDisplay(win);
            break;
    }
}

// Copied from https://learnopengl.com/In-Practice/Debugging
void APIENTRY glDebugOutput(GLenum source,
                            GLenum type,
                            unsigned int id,
                            GLenum severity,
                            GLsizei length,
                            const char *message,
                            const void *userParam) {
    // ignore non-significant error/warning codes
    if (id == 131169 || id == 131185 || id == 131218 || id == 131204) return;

    std::cout << "---------------" << std::endl;
    std::cout << "Debug message (" << id << "): " << message << std::endl;

    switch (source) {
        case GL_DEBUG_SOURCE_API:
            std::cout << "Source: API";
            break;
        case GL_DEBUG_SOURCE_WINDOW_SYSTEM:
            std::cout << "Source: Window System";
            break;
        case GL_DEBUG_SOURCE_SHADER_COMPILER:
            std::cout << "Source: Shader Compiler";
            break;
        case GL_DEBUG_SOURCE_THIRD_PARTY:
            std::cout << "Source: Third Party";
            break;
        case GL_DEBUG_SOURCE_APPLICATION:
            std::cout << "Source: Application";
            break;
        case GL_DEBUG_SOURCE_OTHER:
            std::cout << "Source: Other";
            break;
    }
    std::cout << std::endl;

    switch (type) {
        case GL_DEBUG_TYPE_ERROR:
            std::cout << "Type: Error";
            break;
        case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:
            std::cout << "Type: Deprecated Behaviour";
            break;
        case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR:
            std::cout << "Type: Undefined Behaviour";
            break;
        case GL_DEBUG_TYPE_PORTABILITY:
            std::cout << "Type: Portability";
            break;
        case GL_DEBUG_TYPE_PERFORMANCE:
            std::cout << "Type: Performance";
            break;
        case GL_DEBUG_TYPE_MARKER:
            std::cout << "Type: Marker";
            break;
        case GL_DEBUG_TYPE_PUSH_GROUP:
            std::cout << "Type: Push Group";
            break;
        case GL_DEBUG_TYPE_POP_GROUP:
            std::cout << "Type: Pop Group";
            break;
        case GL_DEBUG_TYPE_OTHER:
            std::cout << "Type: Other";
            break;
    }
    std::cout << std::endl;

    switch (severity) {
        case GL_DEBUG_SEVERITY_HIGH:
            std::cout << "Severity: high";
            break;
        case GL_DEBUG_SEVERITY_MEDIUM:
            std::cout << "Severity: medium";
            break;
        case GL_DEBUG_SEVERITY_LOW:
            std::cout << "Severity: low";
            break;
        case GL_DEBUG_SEVERITY_NOTIFICATION:
            std::cout << "Severity: notification";
            break;
    }
    std::cout << std::endl;
    std::cout << std::endl;
}

int main(int argc, char *argv[]) {
    // Init OpenGL
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);

    // Select OpenGL version
    GLFWwindow *win = nullptr;
    std::array<std::array<int, 2>, 8> openGL_supported_versions = {
            {
                    {4, 6},
                    {4, 5},
                    {4, 4},
                    {4, 3},
                    {4, 2},
                    {4, 1},
                    {4, 0},
                    {3, 3}
            }};

    for (int i = 0; i < openGL_supported_versions.size() && win == nullptr; i++) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGL_supported_versions[i][0]);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGL_supported_versions[i][1]);
        win = glfwCreateWindow(600, 600, "OpenGL", nullptr, nullptr);
    }

    if (win == nullptr)
        return 0;

    // Select window
    glfwMakeContextCurrent(win);

    // Display OpenGL info
    std::cout << "GL: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "SL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    // Init OpenGL
    glewInit();

    // Init debug output
    int flags;
    glGetIntegerv(GL_CONTEXT_FLAGS, &flags);
    if (flags & GL_CONTEXT_FLAG_DEBUG_BIT) {
        // initialize debug output
        glEnable(GL_DEBUG_OUTPUT);
        glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
        glDebugMessageCallback(glDebugOutput, nullptr);
        glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
    } else {
        std::cout << "Debug output not initialized" << std::endl;
    }

    // Init graphical engine
    initEngine();

    // Set callbacks for display and reshape
    glfwSetFramebufferSizeCallback(win, callbackOpenGLReshape);
    glfwSetWindowRefreshCallback(win, callbackOpenGLDisplay);
    glfwSetKeyCallback(win, callbackOpenGLKeyboard);

    // Wait until program ends
    while (!glfwWindowShouldClose(win))
        glfwWaitEvents();

    glfwTerminate();

    return 0;
}
