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

void initEngine() {
    // Load Scene
    // TODO: Change to load with assimp
    OBJ obj;
    obj.load("assets/teapot.obj");

    // Scene description
    SceneDescription sceneDescription;

    // Add meshes
    SceneMesh sceneMesh;
    sceneMesh.vertices = obj.faces();
    sceneMesh.normals = obj.normals();
    sceneDescription.meshes.push_back(sceneMesh);

    std::cout << "Faces " << obj.faces().size() << std::endl;

    // Add lights
    SceneLight sceneLight;
    sceneLight.position = glm::normalize(glm::vec3(1.0f));
    sceneDescription.lights.push_back(sceneLight);

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
        renderFrame(w, h);
    else
        renderFrameWithDeepOfField(w, h);

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
     glfwSetKeyCallback(win, callbackOpenGLKeyboard);

    // Wait until program ends
    while (!glfwWindowShouldClose(win))
        glfwWaitEvents();

    glfwTerminate();

    return 0;
}
