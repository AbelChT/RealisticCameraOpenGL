//
// Created by abel on 28/4/20.
//
#include "SceneRenderer.h"

// Vertex array object
GLuint vertexArrayObject;

// Vertex array object size / Number of vertices
GLuint vertexArrayObjectSize;

// Gourd shader program id
GLuint gourdProgramId;

// Lights info
glm::vec3 light;

// Gourd shader path
const char gourdVsPath[] = "shaders/my_gourd.vert";
const char gourdFsPath[] = "shaders/my_gourd.frag";

void initSceneRenderer(const SceneDescription &sceneDescription) {
    // TODO Warning: From now we are only working with one mesh of the scene and one light
    // Generate and bind vertex array
    glGenVertexArrays(1, &vertexArrayObject);
    glBindVertexArray(vertexArrayObject);
    vertexArrayObjectSize = sceneDescription.meshes.begin()->vertices.size();

    // Associate vertex locations to the VAO
    GLuint vertexLocationBufferObject;
    glGenBuffers(1, &vertexLocationBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexLocationBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexArrayObjectSize * sizeof(glm::vec3),
                 sceneDescription.meshes.begin()->vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);


    // Associate normals to the VAO
    GLuint vertexNormalsBufferObject;
    glGenBuffers(1, &vertexNormalsBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexNormalsBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexArrayObjectSize * sizeof(glm::vec3),
                 sceneDescription.meshes.begin()->normals.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    // TODO Warning: No texture associated from now

    // Load shader
    gourdProgramId = createGLProgram(gourdVsPath, gourdFsPath);

    if (gourdProgramId == 0) {
        std::cerr << "Program was not created for shader: Gourd" << std::endl;
    }

    glClearColor(0, 0, 0, 0);

    // Load lights
    light = sceneDescription.lights.front().position;


    // Configure OpenGL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

}

void reshapeScene(int w, int h) {
    glViewport(0, 0, w, h);
}

void renderFrame(int w, int h) {
    float world_ph = 0.0;
    float world_th = 30.0;
    float world_ro = 1.0;

    if (h <= 0 || w <= 0) return;

    float aspect = float(w) / float(h);

    glm::mat4 pers = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);

    const float ph = glm::radians(world_ph);
    const float th = glm::radians(world_th);

    glm::vec3 axis(cos(ph) * cos(th), sin(ph) * cos(th), sin(th));

    glm::vec3 to(0, 0, 0);
    glm::vec3 eye = to + world_ro * axis;
    glm::mat4 camera = glm::lookAt(eye, to, glm::vec3(0, 0, 1));

    glm::mat4 view = pers * camera;

//	light = axis;
//  glm::vec3 light = eye;
//	light = glm::normalize(glm::vec3(1.0f));
//	light = glm::normalize(glm::vec3(4.0f))

    // Use Gourd
    glUseProgram(gourdProgramId);

    GLuint eye_loc = glGetUniformLocation(gourdProgramId, "eye");
    GLuint view_loc = glGetUniformLocation(gourdProgramId, "view");
    GLuint light_loc = glGetUniformLocation(gourdProgramId, "light");
    GLuint kd_loc = glGetUniformLocation(gourdProgramId, "kd");
    GLuint dcol_loc = glGetUniformLocation(gourdProgramId, "dcol");
    GLuint ks_loc = glGetUniformLocation(gourdProgramId, "ks");
    GLuint scol_loc = glGetUniformLocation(gourdProgramId, "scol");
    GLuint ns_loc = glGetUniformLocation(gourdProgramId, "ns");

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

    glBindVertexArray(vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    const int numberOfBokehIterations = 10;
    const float movementRange = 0.4;

    float axisPosition = world_ro - movementRange / 2;

    for (int i = 0; i < numberOfBokehIterations; ++i) {
        glm::vec3 eyeInternal = to + axisPosition * axis;
        glUniform3fv(eye_loc, 1, glm::value_ptr(eyeInternal));

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSize);
        glAccum(GL_ACCUM, 1.0 / (float)numberOfBokehIterations);

        axisPosition += movementRange / (float)numberOfBokehIterations;
    }
    glAccum(GL_RETURN, 1.0);

//	glFlush();

}