//
// Created by abel on 28/4/20.
//
#include "SceneRenderer.h"
#include "base_code/png.h"

// Vertex array object
GLuint vertexArrayObject;

// Vertex array object size / Number of vertices
GLuint vertexArrayObjectSize;

// Gourd shader program id
GLuint gourdProgramId;

// Lights info
glm::vec3 light;

// Gourd shader path
const char gourdVsPath[] = "shaders/myGourd.vert";
const char gourdFsPath[] = "shaders/myGourd.frag";

//// Buffer user for accumulations
//GLuint accumulatorFrameBufferObject;

// Transfer from texture to the frameBuffer
GLuint transferProgramId;

// Transfer shader path
const char accumulateVsPath[] = "shaders/sumTextures.vert";
const char accumulateFsPath[] = "shaders/sumTextures.frag";

// screen texture VAO
GLuint screenTextureVAO;

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

    // Load transfer shader
    gourdProgramId = createGLProgram(gourdVsPath, gourdFsPath);

    if (gourdProgramId == 0) {
        std::cerr << "Program was not created for shader: Gourd" << std::endl;
    }

    glClearColor(0, 0, 0, 0);
    // glClearAccum(0, 0, 0, 0);

    // Load lights
    light = sceneDescription.lights.front().position;

    // Configure OpenGL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

//    // Create buffer to do accumulation. Must be update with each screen resize
//    int w = 384;
//    int h = 256;

//    // Save actual user buffers
//    GLint defaultDrawFboId, defaultReadFboId;
//    glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING, &defaultDrawFboId);
//    glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING, &defaultReadFboId);

//    // Generate frame buffer to emulate accumulator buffer
//    glGenFramebuffers(1, &accumulatorFrameBufferObject);
//    glBindFramebuffer(GL_FRAMEBUFFER, accumulatorFrameBufferObject);
//
//    // Generate texture for the accumulator buffer
//    GLuint accumulatorTexColorBuffer;
//    glGenTextures(1, &accumulatorTexColorBuffer);
//    glBindTexture(GL_TEXTURE_2D, accumulatorTexColorBuffer);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, 0);
//
//    // Attach texture to currently bound frame buffer object
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, accumulatorTexColorBuffer, 0);
//
//    GLuint accumulatorRenderBuffer;
//    glGenRenderbuffers(1, &accumulatorRenderBuffer);
//    glBindRenderbuffer(GL_RENDERBUFFER, accumulatorRenderBuffer);
//    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, w, h);
//    glBindRenderbuffer(GL_RENDERBUFFER, 0);
//
//    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, accumulatorRenderBuffer);
//
//    // Check if frame buffer created
//    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
//        std::cerr << "ERROR FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
//
//    // Restore previous buffers
//    glBindFramebuffer(GL_READ_FRAMEBUFFER, defaultReadFboId);
//    glBindFramebuffer(GL_DRAW_FRAMEBUFFER, defaultDrawFboId);

    // Create plane to render texture in the entire screen
    const glm::vec2 screenTextureVertices[] = {
            glm::vec2(-1.0f, 1.0f),
            glm::vec2(-1.0f, -1.0f),
            glm::vec2(1.0f, -1.0f),

            glm::vec2(-1.0f, 1.0f),
            glm::vec2(1.0f, -1.0f),
            glm::vec2(1.0f, 1.0f)
    };

    const glm::vec2 screenTextureTexture[] = {
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 0.0f),

            glm::vec2(0.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(1.0f, 1.0f)
    };

    // screen texture VAO
    GLuint screenTextureTextureVBO, screenTextureVerticesVBO;
    glGenVertexArrays(1, &screenTextureVAO);
    glBindVertexArray(screenTextureVAO);

    // Associate vertex locations to the VAO
    glGenBuffers(1, &screenTextureVerticesVBO);
    glBindBuffer(GL_ARRAY_BUFFER, screenTextureVerticesVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenTextureVertices), &screenTextureVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    glGenBuffers(1, &screenTextureTextureVBO);
    glBindBuffer(GL_ARRAY_BUFFER, screenTextureTextureVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(screenTextureTexture), &screenTextureTexture, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), nullptr);

    // Load transfer shader
    transferProgramId = createGLProgram(accumulateVsPath, accumulateFsPath);

    if (transferProgramId == 0) {
        std::cerr << "Program was not created for shader: Transfer" << std::endl;
    }

}

void reshapeScene(int w, int h) {
    glViewport(0, 0, w, h);
}

/**
 * Render frame
 * @param w
 * @param h
 * @param world_ro
 */
void renderFrameIntoDefaultFrameBuffer(int w, int h, float world_ro = 1.0) {
    std::cout << "renderFrameIntoDefaultFrameBuffer" << std::endl;

    float world_ph = 0.0;
    float world_th = 30.0;

    if (h <= 0 || w <= 0) return;

    float aspect = float(w) / float(h);

    glm::mat4 pers = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);

    const float ph = glm::radians(world_ph);
    const float th = glm::radians(world_th);

    glm::vec3 axis(cos(ph) * cos(th), sin(ph) * cos(th), sin(th));

    glm::vec3 to(0, 0, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

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

    glUniform3fv(light_loc, 1, glm::value_ptr(light));
    glUniform1f(kd_loc, 0.5f);
    glUniform3f(dcol_loc, 112 / 255.0, 175 / 255.0, 55 / 255.0);
    glUniform1f(ks_loc, 0.5f);
    glUniform3f(scol_loc, 1.0, 1.0, 1.0);
    glUniform1f(ns_loc, 10.0f);

    glBindVertexArray(vertexArrayObject);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Cube 1
    float axisPosition = world_ro + 2.0f;

    glm::vec3 eye = to + axisPosition * axis;

    glm::mat4 camera = glm::lookAt(eye, to, glm::vec3(0, 0, 1));

    glm::mat4 view = pers * camera;

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));

    glUniform3fv(eye_loc, 1, glm::value_ptr(eye));
    glUniform3f(dcol_loc, 255 / 255.0, 0 / 255.0, 0 / 255.0);
    glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSize);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

void accumulateTexturesIntoDefaultFrameBuffer(GLuint accumulatorTexColorBuffer, unsigned int numberOfFrames) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(transferProgramId);
    glBindVertexArray(screenTextureVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Communicate the number of frames in the texture
    GLuint numberOfFramesLoc = glGetUniformLocation(transferProgramId, "numberOfFrames");
    glUniform1i(numberOfFramesLoc, numberOfFrames);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}


void renderFrame(int w, int h) {
    const int numberOfFrames = 4;
    GLuint accumulatorTexColorBuffer;
    glGenTextures(1, &accumulatorTexColorBuffer);
    glActiveTexture(GL_TEXTURE0);

    // Generate texture for the accumulator buffer
    glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, w, h, numberOfFrames);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    for (int i = 0; i < numberOfFrames; i++) {
        renderFrameIntoDefaultFrameBuffer(w, h, 1.0f + ((float) i) * 0.4f);
        glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);
        glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 0, 0, w, h);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    accumulateTexturesIntoDefaultFrameBuffer(accumulatorTexColorBuffer, numberOfFrames);

    glDeleteTextures(1, &accumulatorTexColorBuffer);
}
