//
// Created by abel on 27/4/20.
//
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <array>
#include <iostream>

void initEngine() {

}

void callbackOpenGLReshape(GLFWwindow *win, int w, int h) {
    glfwMakeContextCurrent(win);
    std::cout << "Callback: Refresh" << std::endl;
    // world_reshape(w, h);
}

void callbackOpenGLDisplay(GLFWwindow *win) {
    std::cout << "Callback: Display" << std::endl;
    int w, h;
    glfwGetWindowSize(win, &w, &h);

    glfwMakeContextCurrent(win);

    // world_display(w, h);

    glfwSwapBuffers(win);
}

int main(int argc, char *argv[]) {
    // Init OpenGL
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

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
        win = glfwCreateWindow(384, 256, "OpenGL", nullptr, nullptr);
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

    // Init graphical engine
    initEngine();

    // Set callbacks for display and reshape
    glfwSetFramebufferSizeCallback(win, callbackOpenGLReshape);
    glfwSetWindowRefreshCallback(win, callbackOpenGLDisplay);

    // Wait until program ends
    while (!glfwWindowShouldClose(win))
        glfwWaitEvents();

    glfwTerminate();

    return 0;
}
