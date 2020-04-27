//
// Created by abel on 27/4/20.
//

#ifndef TESTINGOPENGL_OPENGL_UTILS_H
#define TESTINGOPENGL_OPENGL_UTILS_H

#include <string>
#include <fstream>
#include <iostream>
#include <GL/glew.h>

/**
 * Load a shader from a file
 * @param file_path Path of the shader
 * @return Shader as string
 */
std::string readShaderFromFile(const char file_path[]);

/**
 * Create GLProgram and return it
 * @param vertex_shader_file_path Vertex shader file path
 * @param fragment_shader_file_path  Fragment shader file path
 * @return OpenGL program on succeed and 0 otherwise
 */
GLuint createGLProgram(const char vertex_shader_file_path[], const char fragment_shader_file_path[]);

#endif //TESTINGOPENGL_OPENGL_UTILS_H
