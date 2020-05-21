//
// Created by abel on 27/4/20.
//
#include "OpenglUtils.h"

#include <fstream>
#include <iostream>

/**
 * Load a shader from a file
 * @param file_path Path of the shader
 * @return Shader as string
 */
std::string readShaderFromFile(const char file_path[]) {
    // Read file
    std::ifstream t(file_path);

    // Check if file is correctly opened
    if (t.is_open()) {
        std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
        return str;
    } else {
        std::cerr << "Error loading the shader file: " << file_path << std::endl;
        return "";
    }
}

/**
 * Create GLProgram and return it
 * @param vertex_shader_file_path Vertex shader file path
 * @param fragment_shader_file_path  Fragment shader file path
 * @return OpenGL program on succeed and o otherwise
 */
GLuint createGLProgram(const char vertex_shader_file_path[], const char fragment_shader_file_path[]) {
    // Load vertex shader
    std::string vertex_shader_src = readShaderFromFile(vertex_shader_file_path);
    const char *vertex_shader_src_char = vertex_shader_src.c_str();

    // Load and compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src_char, nullptr);
    glCompileShader(vertex_shader);

    // Check if shader have been correctly compiled
    GLint compile_error_vs;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_error_vs);

    if (compile_error_vs != GL_TRUE) {
        std::cerr << "Error compiling the vertex shader file: " << vertex_shader_file_path << std::endl;
    }

    // Load fragment shader
    std::string fragment_shader_src = readShaderFromFile(fragment_shader_file_path);
    const char *fragment_shader_src_char = fragment_shader_src.c_str();

    // Load and compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src_char, nullptr);
    glCompileShader(fragment_shader);

    // Check if shader have been correctly compiled
    GLint compile_error_fs;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_error_fs);

    if (compile_error_fs != GL_TRUE) {
        std::cerr << "Error compiling the fragment shader file: " << fragment_shader_file_path << std::endl;
    }

    // Compile the program
    GLuint opengl_program = glCreateProgram();
    glAttachShader(opengl_program, vertex_shader);
    glAttachShader(opengl_program, fragment_shader);
    glLinkProgram(opengl_program);

    // Check if program have been correctly linked
    GLint link_error_program;
    glGetProgramiv(opengl_program, GL_LINK_STATUS, &link_error_program);

    if (link_error_program != GL_TRUE) {
        std::cerr << "Error linking the program with vertex shader file: " << vertex_shader_file_path << std::endl;

        // Delete the program
        glDeleteProgram(opengl_program);
        opengl_program = 0;
    }

    // Delete shader files
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return opengl_program;
}
