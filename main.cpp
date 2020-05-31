//
// Created by abel on 27/4/20.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>
#include <filesystem>
#include "libs/png.h"
#include "SceneRenderer.h"
#include "SceneReader.h"

void initEngine(const char scenePath[]) {
    // Load scene
    auto returned_data = readScene(scenePath);

    // Init engine
    initSceneRenderer(returned_data.value());
}

void callbackOpenGLReshape(GLFWwindow *win, int w, int h) {
    glfwMakeContextCurrent(win);
    std::cout << "Callback: Refresh" << std::endl;
    reshapeScene(w, h);
}

bool style = false;

void callbackOpenGLDisplay(GLFWwindow *win) {
    std::cout << "------------------------------" << std::endl
              << std::endl;
    if (!style)
        std::cout << "Display with pinhole mode" << std::endl;
    else
        std::cout << "Display with realistic camera mode" << std::endl;

    std::cout << std::endl
              << "------------------------------" << std::endl;

    int w, h;
    glfwGetWindowSize(win, &w, &h);

    glfwMakeContextCurrent(win);

    renderFrame(w, h, style);

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
        case GLFW_KEY_S:
            cout << "Callback save image" << endl;
            int w, h;
            glfwGetWindowSize(win, &w, &h);
            vector<float> pixels(w * h * 3);
            glReadPixels(0, 0, w, h, GL_RGB, GL_FLOAT, pixels.data());

            // Create out directory
            auto out_folder = filesystem::path("out");
            create_directory(out_folder);

            // Save image
            PNG lib_png(w, h, pixels, true);
            lib_png.save("out/image.png");
            cout << "Image saved" << endl;
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
    // Scene path
    char *scenePath;

    if (argc != 2) {
        cout << "You must provide a scene file" << std::endl;
        return -1;
    } else {
        scenePath = argv[1];
        cout << "Scene: " << argv[1] << " will be renderized" << std::endl;
    }

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
                    {4, 2}
            }};

    for (int i = 0; i < openGL_supported_versions.size() && win == nullptr; i++) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, openGL_supported_versions[i][0]);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, openGL_supported_versions[i][1]);
        win = glfwCreateWindow(960, 540, "OpenGL", nullptr, nullptr);
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

    // Display instructions
    std::cout << "------------------------------" << std::endl
              << std::endl
              << "Usage instructions:" << std::endl
              << "Key R: Change between pinhole mode and realistic camera mode" << std::endl
              << "Key S: Save image in out/image.png" << std::endl
              << std::endl
              << "------------------------------" << std::endl;

    // Init graphical engine
    initEngine(scenePath);

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
