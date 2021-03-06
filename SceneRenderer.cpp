//
// Created by abel on 28/4/20.
//
#include "SceneRenderer.h"

#include "OpenglUtils.h"
#include "libs/png.h"

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_inverse.hpp>
#include <chrono>
#include <ctime>
#include <cmath>
#include <random>
#include <iostream>

// Vertex array object
std::vector<GLuint> vertexArrayObjects;

// Vertex array object size / Number of vertices
std::vector<GLuint> vertexArrayObjectSizes;

// Texture used in the scene
std::vector<GLuint> texturesScene;

// Lights info
SceneLight sceneLight;
AmbientLight ambientLight;

// Scene camera
SceneCamera sceneCamera;

// Scene objects
std::vector<ObjectDescription> sceneObjects;

// Scene materials
std::vector<SceneMaterial> sceneMaterials;

// Gourd shader program id
GLuint myCustomShaderProgramId;

#define USE_GOURAUD 0

#if USE_GOURAUD

// My custom shader path
const char myCustomShaderVsPath[] = "shaders/myCustomGouraudShader.vert";
const char myCustomShaderFsPath[] = "shaders/myCustomGouraudShader.frag";

#else

// My custom shader path
const char myCustomShaderVsPath[] = "shaders/myCustomPhongShader.vert";
const char myCustomShaderFsPath[] = "shaders/myCustomPhongShader.frag";

#endif

// Transfer from texture to the frameBuffer
GLuint transferWeightedProgramId;

// Transfer weighted shader path
const char accumulateWeightedVsPath[] = "shaders/sumTextures.vert";
const char accumulateWeightedFsPath[] = "shaders/sumWeightedTextures.frag";

// screen texture VAO
GLuint screenTextureVAO;

/**
 * Init the render
 * @param sceneDescription
 */
void initSceneRenderer(const SceneDescription &sceneDescription) {
    // Generate and bind vertex array
    vertexArrayObjects.resize(sceneDescription.meshes.size());
    vertexArrayObjectSizes.resize(sceneDescription.meshes.size());
    glGenVertexArrays(sceneDescription.meshes.size(), vertexArrayObjects.data());

    for (int i = 0; i < vertexArrayObjects.size(); i++) {
        auto vertexArrayObject = vertexArrayObjects[i];
        glBindVertexArray(vertexArrayObject);
        vertexArrayObjectSizes[i] = sceneDescription.meshes[i].normals.size();

        // Associate vertex locations to the VAO
        GLuint vertexLocationBufferObject;
        glGenBuffers(1, &vertexLocationBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexLocationBufferObject);
        glBufferData(GL_ARRAY_BUFFER, vertexArrayObjectSizes[i] * sizeof(glm::vec3),
                     sceneDescription.meshes[i].vertices.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

        // Associate normals to the VAO
        GLuint vertexNormalsBufferObject;
        glGenBuffers(1, &vertexNormalsBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexNormalsBufferObject);
        glBufferData(GL_ARRAY_BUFFER, vertexArrayObjectSizes[i] * sizeof(glm::vec3),
                     sceneDescription.meshes[i].normals.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);

        // Associate texture locations to the VAO
        GLuint vertexTextureBufferObject;
        glGenBuffers(1, &vertexTextureBufferObject);
        glBindBuffer(GL_ARRAY_BUFFER, vertexTextureBufferObject);
        glBufferData(GL_ARRAY_BUFFER, vertexArrayObjectSizes[i] * sizeof(glm::vec3),
                     sceneDescription.meshes[i].texture_positions.data(), GL_STATIC_DRAW);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), nullptr);
    }

    // Save textures
    texturesScene.resize(sceneDescription.textures.size());

    // Generate and bind texture array
    glGenTextures(sceneDescription.textures.size(), texturesScene.data());

    // Select one texture unit to work with
    glActiveTexture(GL_TEXTURE0);

    for (int i = 0; i < sceneDescription.textures.size(); ++i) {
        GLuint textureId = texturesScene[i];
        const unsigned int numberOfMipmaps = 8;

        // Allocate texture space
        glBindTexture(GL_TEXTURE_2D, textureId);
        glTexStorage2D(GL_TEXTURE_2D,
                       numberOfMipmaps,
                       GL_RGB32F,
                       sceneDescription.textures[i].width(), sceneDescription.textures[i].height());
        glTexSubImage2D(GL_TEXTURE_2D,
                        0, 0, 0,
                        sceneDescription.textures[i].width(), sceneDescription.textures[i].height(),
                        GL_RGB,
                        GL_FLOAT,
                        sceneDescription.textures[i].pixels().data());

        // Set texture parameters
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

        // Unbind texture
        glBindTexture(GL_TEXTURE_2D, 0);
    }

    // Save camera
    sceneCamera = sceneDescription.camera;

    // Save scene objects
    sceneObjects = sceneDescription.objects;

    // Save scene materials
    sceneMaterials = sceneDescription.materials;

    // Load shader
    myCustomShaderProgramId = createGLProgram(myCustomShaderVsPath, myCustomShaderFsPath);

    // Load transfer shader
    myCustomShaderProgramId = createGLProgram(myCustomShaderVsPath, myCustomShaderFsPath);

    if (myCustomShaderProgramId == 0) {
        std::cerr << "Program was not created for shader: Gourd" << std::endl;
    }

    glClearColor(0, 0, 0, 0);

    // Load lights
    sceneLight = sceneDescription.light;
    ambientLight = sceneDescription.ambientLight;

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

    // Load transfer weighted shader
    transferWeightedProgramId = createGLProgram(accumulateWeightedVsPath, accumulateWeightedFsPath);

    if (transferWeightedProgramId == 0) {
        std::cerr << "Program was not created for shader: Transfer weighted" << std::endl;
    }
}

/**
 * Callback for scene reshape
 * @param w
 * @param h
 */
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
void renderFrameIntoDefaultFrameBuffer(const int w, const int h, const glm::vec3 &eye, const glm::vec3 &to,
                                       float fieldOfView, float zNear, float zFar,
                                       glm::vec3 up = glm::vec3(0, 0, 1)) {
    // std::cout << "renderFrameIntoDefaultFrameBuffer" << std::endl;

    if (h <= 0 || w <= 0) return;

    float aspect = float(w) / float(h);

    glm::mat4 pers = glm::perspective(glm::radians(fieldOfView), aspect, zNear, zFar);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Use my render program
    glUseProgram(myCustomShaderProgramId);

    GLuint pinholeLoc = glGetUniformLocation(myCustomShaderProgramId, "pinholePosition");
    GLuint modelClipMatrixLoc = glGetUniformLocation(myCustomShaderProgramId, "worldClipMatrix");
    GLuint modelWorldMatrixLoc = glGetUniformLocation(myCustomShaderProgramId, "modelWorldMatrix");
    GLuint lightPositionLoc = glGetUniformLocation(myCustomShaderProgramId, "lightPosition");
    GLuint lightColorLoc = glGetUniformLocation(myCustomShaderProgramId, "lightColor");
    GLuint objectColorLoc = glGetUniformLocation(myCustomShaderProgramId, "objectColor");
    GLuint ambientLightColorLoc = glGetUniformLocation(myCustomShaderProgramId, "ambientLightComponent");
    GLuint normalMatrixLoc = glGetUniformLocation(myCustomShaderProgramId, "normalMatrix");

    GLuint objectShininessLoc = glGetUniformLocation(myCustomShaderProgramId, "objectShininess");
    GLuint objectSpecularStrengthLoc = glGetUniformLocation(myCustomShaderProgramId, "objectSpecularStrength");
    GLuint objectDiffuseStrengthLoc = glGetUniformLocation(myCustomShaderProgramId, "objectDiffuseStrength");

    GLuint textureflg_loc = glGetUniformLocation(myCustomShaderProgramId, "enableTextureFlag");
    // GLuint colorflg_loc = glGetUniformLocation(myCustomShaderProgramId, "enableMaterialFlag");
    GLuint tex_loc = glGetUniformLocation(myCustomShaderProgramId, "tex");

    glUniform3fv(lightPositionLoc, 1, glm::value_ptr(sceneLight.position));

    glUniform3fv(lightColorLoc, 1, glm::value_ptr(sceneLight.color));
    glUniform3fv(ambientLightColorLoc, 1, glm::value_ptr(ambientLight.color));

    glUniform3fv(pinholeLoc, 1, glm::value_ptr(eye));

    // Work with only this texture unit
    glActiveTexture(GL_TEXTURE0);

    // Set texture location
    glUniform1i(tex_loc, 0);

    // Transform from world space to view space
    glm::mat4 camera = glm::lookAt(eye, to, up);

    // Transform from view space to homogeneous space
    glm::mat4 worldClipMatrix = pers * camera;

    glUniformMatrix4fv(modelClipMatrixLoc, 1, GL_FALSE, glm::value_ptr(worldClipMatrix));

    for (auto &sceneObject : sceneObjects) {
        auto vertexArrayObject = vertexArrayObjects[sceneObject.meshIndex];
        glBindVertexArray(vertexArrayObject);
        glEnableVertexAttribArray(0);
        glEnableVertexAttribArray(1);
        glEnableVertexAttribArray(2);

        // Object material selection
        auto objectMaterial = sceneMaterials[sceneObject.materialIndex];
        // glUniform1i(colorflg_loc, true);
        glUniform3f(objectColorLoc, objectMaterial.color.x, objectMaterial.color.y, objectMaterial.color.z);
        glUniform1f(objectShininessLoc, objectMaterial.shininess);
        glUniform1f(objectSpecularStrengthLoc, objectMaterial.specularStrength);
        glUniform1f(objectDiffuseStrengthLoc, objectMaterial.diffuseStrength);

        // Object albedo texture selection
        if (objectMaterial.albedoTextureIndex.has_value()) {
            glBindTexture(GL_TEXTURE_2D, texturesScene[objectMaterial.albedoTextureIndex.value()]);
            glUniform1i(textureflg_loc, true);
        } else {
            glUniform1i(textureflg_loc, false);
        }

        // Transform from model space to world space
        auto modelWorldMatrix = glm::identity<glm::mat4>();
        modelWorldMatrix = glm::translate(modelWorldMatrix, sceneObject.position);
        modelWorldMatrix = glm::rotate(modelWorldMatrix, glm::radians(sceneObject.rotation.x),
                                       glm::vec3(1.0f, 0.0f, 0.0f));
        modelWorldMatrix = glm::rotate(modelWorldMatrix, glm::radians(sceneObject.rotation.y),
                                       glm::vec3(0.0f, 1.0f, 0.0f));
        modelWorldMatrix = glm::rotate(modelWorldMatrix, glm::radians(sceneObject.rotation.z),
                                       glm::vec3(0.0f, 0.0f, 1.0f));
        modelWorldMatrix = glm::scale(modelWorldMatrix, sceneObject.scale);

        glUniformMatrix4fv(modelWorldMatrixLoc, 1, GL_FALSE, glm::value_ptr(modelWorldMatrix));

        // Normal matrix
        glm::mat3 modelToWorldMatrix3(modelWorldMatrix);
        glm::mat3 normalMatrix = glm::inverseTranspose(modelToWorldMatrix3);

        glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));

        glDrawArrays(GL_TRIANGLES, 0, vertexArrayObjectSizes[sceneObject.meshIndex]);

        // Unbind albedo texture
        if (objectMaterial.albedoTextureIndex.has_value()) {
            glBindTexture(GL_TEXTURE_2D, 0);
        }

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
        glBindVertexArray(0);
    }
}

/**
 * Sum different textures into the default frame buffer
 *
 * Operation done:
 * default frame buffer = (accumulatorTexColorBuffer[0] * numberOfAccumulatedFrames +
 * sum(accumulatorTexColorBuffer[1..numberOfNewFrames])) / (numberOfAccumulatedFrames + numberOfNewFrames)
 *
 * @param accumulatorTexColorBuffer buffer where the textures to sum are located
 * @param numberOfNewFrames new frames
 * @param numberOfAccumulatedFrames accumulated frames
 */
void accumulateWeightedTexturesIntoDefaultFrameBuffer(GLuint accumulatorTexColorBuffer, unsigned int numberOfNewFrames,
                                                      unsigned int numberOfAccumulatedFrames) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(transferWeightedProgramId);
    glBindVertexArray(screenTextureVAO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    // Communicate the number of frames in the texture
    GLuint numberOfFramesLoc = glGetUniformLocation(transferWeightedProgramId, "numberOfNewFrames");
    glUniform1i(numberOfFramesLoc, numberOfNewFrames);

    GLuint numberOfAccumulatedFramesLoc = glGetUniformLocation(transferWeightedProgramId, "numberOfAccumulatedFrames");
    glUniform1i(numberOfAccumulatedFramesLoc, numberOfAccumulatedFrames);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
}

/**
 *
 * Render frame with renderFrameIntoDefaultFrameBuffer in different camera locations and average all of them
 *
 * @param w width
 * @param h height
 * @param camera_positions positions of the camera
 * @param numberOfAccumulatedTextures maximum number of textures that can be accumulated
 */
void renderFramesAndAccumulateMovingCamera(const int w, const int h, const vector<glm::vec3> &camera_positions,
                                           const int numberOfAccumulatedTextures,
                                           const std::optional<std::vector<float>> &dynamicFieldOfView = {}) {
    // Total number of images to take
    const int numberOfFrames = camera_positions.size();

    // Static camera parameters
    glm::vec3 to = sceneCamera.lookAt;

    float zNear = sceneCamera.zNear;

    float zFar = sceneCamera.zFar;

    // Number of textures used for frames
    const int numberOfFrameAccumulatedTextures = numberOfAccumulatedTextures - 1;

    GLuint accumulatorTexColorBuffer;
    glGenTextures(1, &accumulatorTexColorBuffer);
    glActiveTexture(GL_TEXTURE0);

    // Generate texture for the accumulator buffer
    glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);
    glTexStorage3D(GL_TEXTURE_2D_ARRAY, 1, GL_RGB8, w, h, numberOfAccumulatedTextures);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

    // Unbind texture
    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);

    for (int i = 0; i < numberOfFrames; i++) {
        float fieldOfView = dynamicFieldOfView.has_value() ? dynamicFieldOfView.value()[i] : sceneCamera.fieldOfView;

        glm::vec3 renderEye = camera_positions[i];
        renderFrameIntoDefaultFrameBuffer(w, h, renderEye, to, fieldOfView, zNear, zFar);

        const int texture_store_position = i % numberOfFrameAccumulatedTextures;
        glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);
        glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, texture_store_position + 1, 0, 0, w, h);

        if (texture_store_position == numberOfFrameAccumulatedTextures - 1) {
            // cout << "Frame: " << i << " accumulated" << endl;
            // Last texture that can be stored
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            accumulateWeightedTexturesIntoDefaultFrameBuffer(accumulatorTexColorBuffer,
                                                             numberOfFrameAccumulatedTextures,
                                                             i - numberOfFrameAccumulatedTextures + 1);

            if (i < numberOfFrames - 1) {
                // cout << "Frame: " << i << " accumulated in 0" << endl;

                // In the last frame it is no need to copy the texture (it is displayed)
                glBindTexture(GL_TEXTURE_2D_ARRAY, accumulatorTexColorBuffer);
                glCopyTexSubImage3D(GL_TEXTURE_2D_ARRAY, 0, 0, 0, 0, 0, 0, w, h);
                glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
            }
        }
    }

    if (numberOfFrames % numberOfFrameAccumulatedTextures != 0) {
        // In case of remaining textures, display them
        int remaining_textures = numberOfFrames % numberOfFrameAccumulatedTextures;
        // Do the remaining accumulation
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        accumulateWeightedTexturesIntoDefaultFrameBuffer(accumulatorTexColorBuffer, remaining_textures,
                                                         numberOfFrames - remaining_textures + 1);
    }

    glDeleteTextures(1, &accumulatorTexColorBuffer);
}

/**
 * Create field of view integrating over the axis where the camera is looking at
 * @param w width
 * @param h height
 */
void renderFrameWithFieldOfViewIntegratingOverCameraAxis(int w, int h) {
    // Total number of images to take
    const int numberOfFrames = 84;

    glm::vec3 to = sceneCamera.lookAt;

    glm::vec3 eye = sceneCamera.position;

    glm::vec3 axis = glm::normalize(sceneCamera.lookAt - sceneCamera.position);

    // Size of step
    float stepSize = (sceneCamera.rotationRadius / (float) (numberOfFrames)) * 5;

    // Position of the camera
    vector<glm::vec3> camera_position(numberOfFrames);

    // Field of view
    vector<float> field_of_view(numberOfFrames);

    // Default distance from object
    float defaultDistanceFromObject = glm::distance(eye, to);

    for (int i = 0; i < numberOfFrames; i++) {
        glm::vec3 localEye = eye + (((float) i) * stepSize) * axis;
        float distanceFromObject = glm::distance(localEye, to);
        double tan_b =
                ((defaultDistanceFromObject) * tan((sceneCamera.fieldOfView * M_PI) / 180.0)) / distanceFromObject;
        auto localFieldOfView = (float) ((atan(tan_b) * 180.0) / M_PI);
        camera_position[i] = localEye;
        field_of_view[i] = localFieldOfView;
    }

    // Total number of textures used
    const int numberOfAccumulatedTextures = 6;

    // Do accumulation
    renderFramesAndAccumulateMovingCamera(w, h, camera_position, numberOfAccumulatedTextures, field_of_view);
}

/**
 * Create field of view integrating over the perimeter of the diaphragm
 * @param w width
 * @param h height
 */
void renderFrameWithFieldOfViewIntegratingOverPerimeter(int w, int h) {
    // Total number of images to take
    const int numberOfFrames = 84;

    glm::vec3 to = sceneCamera.lookAt;

    glm::vec3 eye = sceneCamera.position;

    // Up, right vectors
    glm::vec3 up = glm::vec3(0, 0, 1);

    glm::vec3 right = glm::normalize(glm::cross(to - eye, up));
    glm::vec3 p_up = glm::normalize(glm::cross(to - eye, right));

    // Position of the camera
    vector<glm::vec3> camera_position(numberOfFrames);

    for (int j = 0; j < numberOfFrames; ++j) {
        // Obtain position in the circle
        glm::vec3 positionInCircle =
                right * cosf(j * 2 * M_PI / numberOfFrames) + p_up * sinf(j * 2 * M_PI / numberOfFrames);

        glm::vec3 renderEye = eye + sceneCamera.rotationRadius * positionInCircle;

        // Add to camera positions
        camera_position[j] = renderEye;
    }

    // Total number of textures used
    const int numberOfAccumulatedTextures = 6;

    // Do accumulation
    renderFramesAndAccumulateMovingCamera(w, h, camera_position, numberOfAccumulatedTextures);
}

/**
 * Create field of view integrating over the diaphragm area
 * @param w width
 * @param h height
 */
void renderFrameWithFieldOfViewIntegratingOverCircle(int w, int h) {
    // Total number of images to take
    const int numberOfFrames = 84;

    glm::vec3 to = sceneCamera.lookAt;

    glm::vec3 eye = sceneCamera.position;

    glm::vec3 axis = glm::normalize(sceneCamera.position - sceneCamera.lookAt);

    // obtain a seed from the system clock:
    unsigned seed1 = std::chrono::system_clock::now().time_since_epoch().count();

    // Random generator
    std::default_random_engine generator(seed1);
    std::uniform_real_distribution<double> distribution(0.0, 1.0);

    // Up, right vectors
    glm::vec3 up = glm::vec3(0, 0, 1);

    glm::vec3 right = glm::normalize(glm::cross(to - eye, up));
    glm::vec3 p_up = glm::normalize(glm::cross(to - eye, right));

    // Position of the camera
    vector<glm::vec3> camera_position(numberOfFrames);

    for (int j = 0; j < numberOfFrames; ++j) {
        // Obtain position in the circle
        auto a = (float) (distribution(generator) * 2 * M_PI);
        auto r = (float) (sceneCamera.rotationRadius * sqrt(distribution(generator)));

        float x = r * cosf(a);
        float y = r * sinf(a);

        glm::vec3 positionInCircle = right * x + p_up * y;

        glm::vec3 renderEye = eye + positionInCircle;

        // Add to camera positions
        camera_position[j] = renderEye;
    }

    // Total number of textures used
    const int numberOfAccumulatedTextures = 6;

    // Do accumulation
    renderFramesAndAccumulateMovingCamera(w, h, camera_position, numberOfAccumulatedTextures);
}

/**
 * Render a frame
 * @param w width
 * @param h height
 * @param withDepthOfView enable depth of field if true, disable it otherwise
 */
void renderFrame(int w, int h, bool withDepthOfView) {
    // Start clock
    std::clock_t c_start = std::clock();
    auto t_start = std::chrono::high_resolution_clock::now();

    // Do render
    if (withDepthOfView) {
        renderFrameWithFieldOfViewIntegratingOverCircle(w, h);
    } else {
        renderFrameIntoDefaultFrameBuffer(w, h, sceneCamera.position, sceneCamera.lookAt, sceneCamera.fieldOfView,
                                          sceneCamera.zNear, sceneCamera.zFar);
    }

    // Finish only should be used if we want to measure the time
    glFinish();

    // End clock
    std::clock_t c_end = std::clock();
    auto t_end = std::chrono::high_resolution_clock::now();

    // Print used time
    std::cout << "CPU time used: "
              << 1000.0 * (double) (c_end - c_start) / CLOCKS_PER_SEC
              << " ms\n";
    std::cout << "Wall clock time passed: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(t_end - t_start).count()
              << " ms\n";
}