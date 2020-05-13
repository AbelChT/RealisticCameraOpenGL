//
// Created by abel on 28/4/20.
//
#include "SceneRenderer.h"
#include "base_code/png.h"

// Vertex array object
//GLuint vertexArrayObject;
vector<GLuint> vertexArrayObjects;

// Vertex array object size / Number of vertices
GLuint vertexArrayObjectSize;
vector<GLuint> vertexArrayObjectSizes;

// Gourd shader program id
GLuint gourdProgramId;

// Lights info
glm::vec3 light;

// Gourd shader path
const char gourdVsPath[] = "shaders/myGourd.vert";
const char gourdFsPath[] = "shaders/myGourd.frag";

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

    // Associate texture locations to the VAO
    GLuint vertexTextureBufferObject;
    glGenBuffers(1, &vertexTextureBufferObject);
    glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBufferObject);
    glBufferData(GL_ARRAY_BUFFER, vertexArrayObjectSize * sizeof(glm::vec3),
                 sceneDescription.meshes.begin()->texture_positions.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

    // TODO: Temporal texture loader, improve it
    tex_png.load("tex/checker.png");
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D,
                   8,
                   GL_RGB32F,
                   tex_png.width(), tex_png.height());
    glTexSubImage2D(GL_TEXTURE_2D,
                    0, 0, 0,
                    tex_png.width(), tex_png.height(),
                    GL_RGB,
                    GL_FLOAT,
                    tex_png.pixels().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    ///// TODO: Temporal texture loader, improve it

    // Load shader
    gourdProgramId = createGLProgram(gourdVsPath, gourdFsPath);

    // Load transfer shader
    gourdProgramId = createGLProgram(gourdVsPath, gourdFsPath);

    if (gourdProgramId == 0) {
        std::cerr << "Program was not created for shader: Gourd" << std::endl;
    }

    glClearColor(0, 0, 0, 0);

    // Load lights
    light = sceneDescription.light.position;

    // Configure OpenGL
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

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
    std::cout << w << " " << h << std::endl;
}

/**
 * Render frame
 * @param w
 * @param h
 * @param world_ro
 */
void renderFrameIntoDefaultFrameBuffer(int w, int h, float cameraDistanceFromO = 1.0, float fieldOfView = 45.0f) {
    std::cout << "renderFrameIntoDefaultFrameBuffer" << std::endl;

    if (h <= 0 || w <= 0) return;

    float aspect = float(w) / float(h);

    glm::mat4 pers = glm::perspective(fieldOfView, aspect, 0.01f, 1000.0f);


    glm::vec3 to(0, 0, 0);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use Gourd
    glUseProgram(gourdProgramId);

    GLuint eye_loc = glGetUniformLocation(gourdProgramId, "eye");
    GLuint view_loc = glGetUniformLocation(gourdProgramId, "view");
    GLuint transformation_matrix_loc = glGetUniformLocation(gourdProgramId, "transformation_matrix");
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
    glEnableVertexAttribArray(2);

    std::cout << axis.x << " " << axis.y << " " << axis.z << std::endl;

    glm::vec3 eye = to + cameraDistanceFromO * axis;
    glUniform3fv(eye_loc, 1, glm::value_ptr(eye));

    glm::mat4 camera = glm::lookAt(eye, to, glm::vec3(0, 0, 1));

    glm::mat4 view = pers * camera;

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));


    // Cube 1
    auto transformationCube1 = glm::identity<glm::mat4>();
    glUniformMatrix4fv(transformation_matrix_loc, 1, GL_FALSE, glm::value_ptr(transformationCube1));
    glUniform3f(dcol_loc, 255.0 / 255.0, 30.0 / 255.0, 30.0 / 255.0);
    glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSize);

    // Cube 2
    auto transformationCube2 = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-8, 2, 0));
    glUniformMatrix4fv(transformation_matrix_loc, 1, GL_FALSE, glm::value_ptr(transformationCube2));
    glUniform3f(dcol_loc, 30.0 / 255.0, 255.0 / 255.0, 30.0 / 255.0);
    glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSize);

    // Cube 3
    auto transformationCube3 = glm::translate(glm::identity<glm::mat4>(), glm::vec3(-32, -16, 0));
    glUniformMatrix4fv(transformation_matrix_loc, 1, GL_FALSE, glm::value_ptr(transformationCube3));
    glUniform3f(dcol_loc, 30.0 / 255.0, 30.0 / 255.0, 255.0 / 255.0);
    glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSize);


//
//    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
//
//    glUniform3fv(eye_loc, 1, glm::value_ptr(eye));
//    glUniform3f(dcol_loc, 255.0 / 255.0, 30.0 / 255.0, 30.0 / 255.0);
//    glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSize);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
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


void renderFrameWithFieldOfView(int w, int h) {
    std::clock_t c_start = std::clock();
    auto t_start = std::chrono::high_resolution_clock::now();

    const int numberOfFrames = 16;
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

    float defaultDistanceFromO = 4.0f;
    float defaultFieldOfView = 45.0f;

    float stepSize = 0.00007f;
    // float stepSize = 0.0004f;

    float firstStepDistanceFromO = defaultDistanceFromO - ((float) numberOfFrames / 2.0) * stepSize;


    for (int i = 0; i < numberOfFrames; i++) {
        float distanceFromO = firstStepDistanceFromO + ((float) i) * stepSize;
        double tan_b = ((defaultDistanceFromO) * tan((defaultFieldOfView * M_PI) / 180.0)) / distanceFromO;
        float fieldOfView = (float) ((atan(tan_b) * 180.0) / M_PI);
        std::cout << "Field of view " << fieldOfView << std::endl;
        renderFrameIntoDefaultFrameBuffer(w, h, distanceFromO, fieldOfView);
        glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);
        glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, i, 0, 0, w, h);
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    accumulateTexturesIntoDefaultFrameBuffer(accumulatorTexColorBuffer, numberOfFrames);

    glDeleteTextures(1, &accumulatorTexColorBuffer);

    std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();

    // Print used time
    std::cout << "CPU time used: "
              << 1000.0 * (c_end - c_start) / CLOCKS_PER_SEC
              << " ms\n";
    std::cout << "Wall clock time passed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count()
              << " ms\n";
}

void renderFrame(int w, int h, bool withFieldOfView) {
    if (withFieldOfView) {
        renderFrameWithFieldOfView(w, h);
    } else {
        renderFrameIntoDefaultFrameBuffer(w, h, 4.0f);
    }
}