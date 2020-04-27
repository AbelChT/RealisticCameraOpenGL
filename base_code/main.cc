#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <iostream>
#include "world.h"

void ogl_info(GLFWwindow *win);

void ogl_init(GLFWwindow *win);

void ogl_reshape(GLFWwindow *win, int width, int height);

void ogl_display(GLFWwindow *win);

void keyboard(GLFWwindow *win, int key, int s, int act, int mod);

void mouse(GLFWwindow *win, int but, int act, int mod);

void motion(GLFWwindow *win, double x, double y);

void scroll(GLFWwindow *win, double x, double y);

using namespace std;

int main(int argc, char *argv[]) {
    glfwInit();

    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow *win = nullptr;
    vector<int> vers = {46, 45, 44, 43, 42, 41, 40, 33};
    for (auto v : vers) {
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, v / 10);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, v % 10);
        win = glfwCreateWindow(384, 256, "OpenGL", nullptr, nullptr);
        if (win != nullptr)
            break;
    }
    if (win == nullptr)
        return 0;

    ogl_info(win);

    ogl_init(win);

    glfwSetFramebufferSizeCallback(win, ogl_reshape);
    glfwSetWindowRefreshCallback(win, ogl_display);

    glfwSetKeyCallback(win, keyboard);
    glfwSetMouseButtonCallback(win, mouse);
    glfwSetScrollCallback(win, scroll);

    while (!glfwWindowShouldClose(win)) {
        glfwWaitEvents();
    }

    glfwTerminate();

    return 0;
}

void ogl_info(GLFWwindow *win) {
    glfwMakeContextCurrent(win);

    cout << "GL: " << glGetString(GL_VERSION) << endl;
    cout << "SL: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << endl;
    cout << flush;
}

void ogl_init(GLFWwindow *win) {
    glfwMakeContextCurrent(win);

    glewInit();

    world_init();
}

void ogl_reshape(GLFWwindow *win, int w, int h) {

    cout << "Refresh" << endl;
    glfwMakeContextCurrent(win);

    world_reshape(w, h);
}

void ogl_display(GLFWwindow *win) {
    cout << "Display" << endl;
    int w, h;
    glfwGetWindowSize(win, &w, &h);

    glfwMakeContextCurrent(win);

    world_display(w, h);

    glfwSwapBuffers(win);
}

void keyboard(GLFWwindow *win, int key, int s, int act, int mod) {
    if (act == GLFW_RELEASE)
        return;

    switch (key) {
        case GLFW_KEY_Q:
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(win, GL_TRUE);
            break;
        case GLFW_KEY_LEFT:
            world_ph += 1;
            break;
        case GLFW_KEY_RIGHT:
            world_ph -= 1;
            break;
        case GLFW_KEY_DOWN:
            world_th += 1;
            if (world_th > 89)
                world_th = 89;
            break;
        case GLFW_KEY_UP:
            world_th -= 1;
            if (world_th < -89)
                world_th = -89;
            break;
        case GLFW_KEY_A:
            world_ro *= 0.9;
            break;
        case GLFW_KEY_Z:
            world_ro *= 1.1;
            break;
        case GLFW_KEY_R:
            world_reset_cam();
            break;
        case GLFW_KEY_F:
            world_fill = !world_fill;
            break;
        case GLFW_KEY_S:
            world_pps = !world_pps;
            break;
        case GLFW_KEY_T:
            world_tex = !world_tex;
            break;
        case GLFW_KEY_E:
            world_env = !world_env;
            break;
        default:
            cout << "key " << key << "<" << char(key) << ">" << endl;
            break;
    }

    ogl_display(win);
}

double lx, ly;

void mouse(GLFWwindow *win, int but, int act, int mod) {
    switch (act) {
        case GLFW_PRESS:
            glfwGetCursorPos(win, &lx, &ly);
//			glfwSetInputMode(win,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
            glfwSetCursorPosCallback(win, motion);
            break;
        case GLFW_RELEASE:
            glfwSetCursorPosCallback(win, 0);
//			glfwSetInputMode(win,GLFW_CURSOR,GLFW_CURSOR_NORMAL);
            break;
    }

    ogl_display(win);
}

void motion(GLFWwindow *win, double x, double y) {
    int w, h;
    glfwGetWindowSize(win, &w, &h);

    double dx = (x - lx) / float(w);
    double dy = (y - ly) / float(h);
    lx = x;
    ly = y;
    world_ph -= 180 * dx;
    world_th += 90 * dy;
    if (world_th > 89)
        world_th = 89;
    if (world_th < -89)
        world_th = -89;

    ogl_display(win);
}

void scroll(GLFWwindow *win, double, double z) {
    if (z < 0)
        world_ro *= 0.9;
    else
        world_ro *= 1.1;

    ogl_display(win);
}
