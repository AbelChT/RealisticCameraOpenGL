#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#include <string>
#include <fstream>
#include <streambuf>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "world.h"
#include "obj.h"
#include "png.h"

using namespace std;

// gourd program id
GLuint gourd_prog;

// gourd shaders path
const char gourd_vs_path[] = "shaders/gourd.vert";
const char gourd_fs_path[] = "shaders/gourd.frag";

// phong program id
GLuint phong_prog;

// phong shaders path
const char phong_vs_path[] = "shaders/phong.vert";
const char phong_fs_path[] = "shaders/phong.frag";

glm::mat4 view;
GLuint view_loc;
glm::vec3 light;
GLuint eye_loc;
GLuint light_loc;
GLuint kd_loc;
GLuint dcol_loc;
GLuint ks_loc;
GLuint scol_loc;
GLuint ns_loc;

bool world_fill = true;
bool world_pps = false;
bool world_tex = false;
GLuint texflg_loc;
bool world_env = false;
GLuint envflg_loc;

OBJ obj;
GLuint vao;
GLuint vao_sz;

PNG tex_png;
GLuint tex;
PNG env_png;
GLuint env;

void glcheck(const string &msg) {
    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR)
        cout << msg << " error: " << gluErrorString(err) << endl;
}

/**
 * Load a shader from a file
 * @param file_path Path of the shader
 * @return Shader as string
 */
string readShaderFromFile(const char file_path[]) {
    // Read file
    ifstream t(file_path);

    // Check if file is correctly opened
    if (t.is_open()) {
        string str((istreambuf_iterator<char>(t)), istreambuf_iterator<char>());
        return str;
    } else {
        cerr << "Error loading the shader file: " << file_path << endl;
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
    string vertex_shader_src = readShaderFromFile(vertex_shader_file_path);
    const char *vertex_shader_src_char = vertex_shader_src.c_str();

    // Load and compile vertex shader
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_shader_src_char, nullptr);
    glCompileShader(vertex_shader);

    // Check if shader have been correctly compiled
    GLint compile_error_vs;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &compile_error_vs);

    if (compile_error_vs != GL_TRUE) {
        cerr << "Error compiling the vertex shader file: " << vertex_shader_file_path << endl;
    }

    // Load fragment shader
    string fragment_shader_src = readShaderFromFile(fragment_shader_file_path);
    const char *fragment_shader_src_char = fragment_shader_src.c_str();

    // Load and compile fragment shader
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_shader_src_char, nullptr);
    glCompileShader(fragment_shader);

    // Check if shader have been correctly compiled
    GLint compile_error_fs;
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &compile_error_fs);

    if (compile_error_fs != GL_TRUE) {
        cerr << "Error compiling the fragment shader file: " << fragment_shader_file_path << endl;
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
        cerr << "Error linking the program with vertex shader file: " << vertex_shader_file_path << endl;

        // Delete the program
        glDeleteProgram(opengl_program);
        opengl_program = 0;
    }

    // Delete shader files
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return opengl_program;
}

void world_init() {
    // glm::mat4 xf = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    obj.load("assets/teapot.obj");
//	obj.load("../model/bb8.obj");
//	obj.load("../model/teapot.obj",xf);
//	obj.load("../model/dragon.obj",xf);
//	obj.load("../model/sphere.obj");
//	obj.load("../model/venus.obj",xf);
//	obj.load("../model/bunny.obj",xf);
//	obj.load("../model/armadillo.obj",xf);
//	obj.load("../model/tyra.obj",xf);
//	obj.load("../model/nefertiti.obj");

    cout << obj.faces().size() / 3 << endl;

    vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vao_sz = obj.faces().size();

    GLuint vpbo = 0;
    glGenBuffers(1, &vpbo);
    glBindBuffer(GL_ARRAY_BUFFER, vpbo);
    glBufferData(GL_ARRAY_BUFFER, vao_sz * sizeof(glm::vec3), obj.faces().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    GLuint vnbo = 0;
    glGenBuffers(1, &vnbo);
    glBindBuffer(GL_ARRAY_BUFFER, vnbo);
    glBufferData(GL_ARRAY_BUFFER, vao_sz * sizeof(glm::vec3), obj.normals().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    GLuint vtbo = 0;
    glGenBuffers(1, &vtbo);
    glBindBuffer(GL_ARRAY_BUFFER, vtbo);
    glBufferData(GL_ARRAY_BUFFER, vao_sz * sizeof(glm::vec3), obj.texcoord().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    // Create phong program
    phong_prog = createGLProgram(phong_vs_path, phong_fs_path);

    if (phong_prog == 0) {
        cerr << "Program was not created for shader: phong" << endl;
    }

    // Create gourd program
    gourd_prog = createGLProgram(gourd_vs_path, gourd_fs_path);

    if (gourd_prog == 0) {
        cerr << "Program was not created for shader: gourd" << endl;
    }

    glClearColor(0, 0, 0, 0);

//	tex_png.load("../tex/paper.png");
    tex_png.load("../tex/checker.png");
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D,
                   8,
                   GL_RGB32F,
                   tex_png.width(), tex_png.height());
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0, 0,
                    tex_png.width(), tex_png.height(),
                    GL_RGB,
                    GL_FLOAT,
                    tex_png.pixels().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    env_png.load("../env/pano/uffizi.png");
//	env_png.load("../env/pano/city.png");
    glGenTextures(1, &env);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, env);
    glTexStorage2D(GL_TEXTURE_2D,
                   8,
                   GL_RGB32F,
                   env_png.width(), env_png.height());
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0, 0,
                    env_png.width(), env_png.height(),
                    GL_RGB,
                    GL_FLOAT,
                    env_png.pixels().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void world_reshape(int w, int h) {
    glViewport(0, 0, w, h);
}


float world_ph = 0.0;
float world_th = 30.0;
float world_ro = 1.0;

void world_reset_cam() {
    world_ph = 0.0;
    world_th = 30.0;
    world_ro = 1.0;
}

void world_display(int w, int h) {
    if (h <= 0) return;
    if (w <= 0) return;

    float aspect = float(w) / float(h);

    glm::mat4 pers = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);

    const float ph = glm::radians(world_ph);
    const float th = glm::radians(world_th);

    glm::vec3 axis(cos(ph) * cos(th), sin(ph) * cos(th), sin(th));

    glm::vec3 to(0, 0, 0);
    glm::vec3 eye = to + world_ro * axis;
    glm::mat4 camera = glm::lookAt(eye, to, glm::vec3(0, 0, 1));

    view = pers * camera;

//	light = axis;
//    light = eye;
    light = glm::normalize(glm::vec3(1.0f));
//	light = glm::normalize(glm::vec3(1.0f));
//	light = glm::normalize(glm::vec3(4.0f));

    glPolygonMode(GL_FRONT_AND_BACK, (world_fill ? GL_FILL : GL_LINE));
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint prg = (world_pps ? phong_prog : gourd_prog);
    glUseProgram(prg);
    eye_loc = glGetUniformLocation(prg, "eye");
    view_loc = glGetUniformLocation(prg, "view");
    light_loc = glGetUniformLocation(prg, "light");
    kd_loc = glGetUniformLocation(prg, "kd");
    dcol_loc = glGetUniformLocation(prg, "dcol");
    ks_loc = glGetUniformLocation(prg, "ks");
    scol_loc = glGetUniformLocation(prg, "scol");
    ns_loc = glGetUniformLocation(prg, "ns");
    texflg_loc = glGetUniformLocation(prg, "texflg");
    envflg_loc = glGetUniformLocation(prg, "envflg");

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(eye_loc, 1, glm::value_ptr(eye));
    glUniform3fv(light_loc, 1, glm::value_ptr(light));
    glUniform1f(kd_loc, 0.5f);
//	glUniform3f(dcol_loc,1.0,1.0,1.0);
    glUniform3f(dcol_loc, 212 / 255.0, 175 / 255.0, 55 / 255.0);
    glUniform1f(ks_loc, 0.5f);
    glUniform3f(scol_loc, 1.0, 1.0, 1.0);
//	glUniform3f(scol_loc,212/255.0,175/255.0,55/255.0);
    glUniform1f(ns_loc, 10.0f);
    glUniform1i(texflg_loc, world_tex);
    glUniform1i(envflg_loc, world_env);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, obj.faces().size());

//	glFlush();
}

void world_clean() {
}
