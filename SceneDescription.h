//
// Created by abel on 26/4/20.
//

#ifndef TESTINGOPENGL_SCENEDESCRIPTION_H
#define TESTINGOPENGL_SCENEDESCRIPTION_H

#include "libs/png.h"

#include <vector>
#include <glm/vec3.hpp>
#include <optional>

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

    // Constructors
    SceneMesh() = default;

    SceneMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals, vector<glm::vec3> texturePositions);

    // Default destructor
    ~SceneMesh() = default;
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

    // Default destructor
    ~SceneCamera() = default;
};

// Parameters that define a camera in the real world
struct CameraDefinition {
public:
    // Position of the scene camera (meters)
    glm::vec3 position;

    // Look at position (meters)
    glm::vec3 lookAt;

    // Horizontal large of the film (millimeters)
    float sensorSize;

    // Distance between the lens and the film (millimeters)
    float focalLength;

    // f-stop (units)
    float fStop;

    // Furthest distance the camera capture (meters)
    float zFar;

    // Constructor
    CameraDefinition(const glm::vec3 &position, const glm::vec3 &lookAt, float sensorSize, float focalLength,
                     float fStop, float zFar);

    // Default destructor
    ~CameraDefinition() = default;
};

// Represent a light in the scene
struct SceneLight {
public:
    // Position of the scene light
    glm::vec3 position;

    // Constructor
    explicit SceneLight(const glm::vec3 &position);

    // Default destructor
    ~SceneLight() = default;
};

// Represent a material in the scene
struct SceneMaterial {
public:
    // Color of the material
    glm::ivec3 color;

    // Constructor
    explicit SceneMaterial(const glm::ivec3 &color);

    // Default destructor
    ~SceneMaterial() = default;
};

// Represent a object in the scene
struct ObjectDescription {
public:
    // Transformation order translate-rotate-scale
    // Position of the object
    glm::vec3 position;

    // Rotation of the object. Order rotation in x, rotation in y, rotation in z
    glm::vec3 rotation;

    // Scale of the object
    glm::vec3 scale;

    // Texture index of the object
    std::optional<unsigned int> textureIndex;

    // Mesh index of the object
    std::optional<unsigned int> meshIndex;

    // Constructor
    ObjectDescription(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale,
                      std::optional<unsigned int> textureIndex, std::optional<unsigned int> meshIndex);

    // Default destructor
    ~ObjectDescription() = default;
};

// Description of the scene
struct SceneDescription {
public:
    // Meshes in the scene
    vector<SceneMesh> meshes;

    // Textures in the scene
    vector<PNG> textures;

    // Objects in the scene
    vector<ObjectDescription> objects;

    // Light in the scene (only one from now)
    SceneLight light;

    // Camera in the scene
    SceneCamera camera;

    // Constructor
    SceneDescription(vector<SceneMesh> meshes, vector<PNG> textures, vector<ObjectDescription> objects,
                     const SceneLight &lights, const SceneCamera &camera);

    // Default destructor
    ~SceneDescription() = default;
};

/**
 * Transform a camera definition to a sceneCamera
 * @param cameraDefinition
 * @return
 */
SceneCamera cameraDefinitionToSceneCamera(CameraDefinition cameraDefinition);

#endif //TESTINGOPENGL_SCENEDESCRIPTION_H
