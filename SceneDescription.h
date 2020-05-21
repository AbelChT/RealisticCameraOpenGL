//
// Created by abel on 26/4/20.
//

#ifndef TESTINGOPENGL_SCENEDESCRIPTION_H
#define TESTINGOPENGL_SCENEDESCRIPTION_H

#include "libs/png.h"

#include <vector>
#include <glm/vec3.hpp>

using namespace std;

// Represent an object in the scene
struct SceneMesh {
public:
    // Vertices of the scene
    vector<glm::vec3> vertices;

    // Normals of the scene
    vector<glm::vec3> normals;

    // Texture positions of the scene
    vector<glm::vec3> texture_positions;

    // Constructor
    SceneMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals,
              vector<glm::vec3> texturePositions);
};


// Represent a camera in the scene
struct SceneCamera {
public:
    // Position of the scene camera
    glm::vec3 position;

    // Look at
    glm::vec3 lookAt;

    // Field of view
    float fieldOfView;

    // zNear
    float zNear;

    // zFar
    float zFar;

    // Rotation radius
    float rotationRadius;

    // Constructor
    explicit SceneCamera();

    // Constructor
    SceneCamera(const glm::vec3 &position, const glm::vec3 &lookAt, float fieldOfView, float zNear, float zFar,
                float rotationRadius);
};

// Parameters that define a camera in the real world
struct CameraDefinition {
public:
    // Position of the scene camera (meters)
    glm::vec3 position;

    // Look at position (meters)
    glm::vec3 lookAt;

    // TODO: Delete lookAt and add this two parameters. With them we can obtain lookAt and up
//    // Rotation of the camera (degrees)
//    glm::vec3 cameraRotation;
//
//    // Distance of the object on focus (meters)
//    float focusDistance;

    // Horizontal large of the film (millimeters)
    float sensorSize;

    // Distance between the lens and the film (millimeters)
    float focalLength;

    // f-stop (units)
    float fStop;

    // Furthest distance the camera capture (meters)
    float zFar;

    CameraDefinition(const glm::vec3 &position, const glm::vec3 &lookAt, float sensorSize, float focalLength,
                     float fStop, float zFar);
};

// Represent a light in the scene
struct SceneLight {
public:
    // Position of the scene light
    glm::vec3 position;

    // Constructor
    explicit SceneLight(const glm::vec3 &position);
};

struct ObjectDescription {
public:
    // Transformation order translate-rotate-scale
    // Position of the object
    glm::vec3 position;

    // Rotation of the object. Order rotation in x, rotation in y, rotation in z
    glm::vec3 rotation;

    // Scale of the object
    glm::vec3 scale;

    // Color of the object
    glm::vec3 color;

    // Use color
    bool useColor;

    // Texture index of the object
    // unsigned int textureIndex;

    // Use texture
    bool useTexture;

    // Mesh index of the object
    unsigned int meshIndex;

    // Constructor
    ObjectDescription(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale,
                      const glm::ivec3 &color, bool useColor, bool useTexture,
                      unsigned int meshIndex);
};

// Description of the scene
struct SceneDescription {
public:
    // Meshes in the scene
    vector<SceneMesh> meshes;

    // Textures in the scene
    // vector<PNG> textures;
    // TODO to improve: Include multiple textures
    PNG texture; // Only one texture from now

    // Objects in the scene
    vector<ObjectDescription> objects;

    // Light in the scene
    SceneLight light;

    // Camera in the scene
    SceneCamera camera;

    // Constructor
    SceneDescription(vector<SceneMesh> meshes, PNG texture, vector<ObjectDescription> objects,
                     const SceneLight &light, const SceneCamera &camera);
};

/**
 * Transform a camera definition to a sceneCamera
 * @param cameraDefinition
 * @return
 */
SceneCamera cameraDefinitionToSceneCamera(CameraDefinition cameraDefinition);

#endif //TESTINGOPENGL_SCENEDESCRIPTION_H
