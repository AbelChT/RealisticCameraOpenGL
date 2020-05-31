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
    // Position of the scene camera (meters)
    glm::vec3 position = glm::vec3(0, 0, 0);

    // Look at (meters)
    glm::vec3 lookAt = glm::vec3(0, 0, 0);

    // Field of view (meters)
    float fieldOfView = 0.0f;

    // zNear (meters)
    float zNear = 0.0f;

    // zFar (meters)
    float zFar = 0.0f;

    // Rotation radius (meters)
    float rotationRadius = 0.0f;

    // Constructor
    SceneCamera() = default;

    SceneCamera(const glm::vec3 &position, const glm::vec3 &lookAt, float fieldOfView, float zNear, float zFar,
                float rotationRadius);

    // Default destructor
    ~SceneCamera() = default;
};

// Parameters that define a camera in the real world
struct CameraDefinition {
public:
    // Position of the scene camera (meters)
    glm::vec3 position = glm::vec3(0, 0, 0);

    // Look at position (meters)
    glm::vec3 lookAt = glm::vec3(0, 0, 0);

    // Horizontal large of the film (millimeters)
    float sensorSize = 0.0f;

    // Distance between the lens and the film (millimeters)
    float focalLength = 0.0f;

    // f-stop (units)
    float fStop = 0.0f;

    // Furthest distance the camera capture (meters)
    float zFar = 0.0f;

    // Constructor
    CameraDefinition(const glm::vec3 &position, const glm::vec3 &lookAt, float sensorSize, float focalLength,
                     float fStop, float zFar);

    // Default destructor
    ~CameraDefinition() = default;
};

// Represent a light in the scene
struct SceneLight {
public:
    // Position of the scene light (meters)
    glm::vec3 position = glm::vec3(0, 0, 0);

    // Intensity in each of the components of the scene light (range [0,1])
    glm::vec3 color = glm::vec3(0.5, 0.5, 0.5);

    // Constructor
    SceneLight() = default;

    SceneLight(const glm::vec3 &position, const glm::vec3 &color);

    // Default destructor
    ~SceneLight() = default;
};

// Represent the ambient light in the scene
struct AmbientLight {
public:
    // Intensity in each of the components of the ambient scene light RGB (range [0,1])
    glm::vec3 color = glm::vec3(0, 0, 0);

    // Constructor
    AmbientLight() = default;

    explicit AmbientLight(const glm::vec3 &color);

    // Default destructor
    ~AmbientLight() = default;
};

// Represent a material in the scene
struct SceneMaterial {
public:
    // Intensity in each of the components of the material color RGB (range [0,1])
    glm::vec3 color = glm::vec3(0, 0, 0);

    // Shininess of the material (32 is a good value)
    float shininess = 32.0f;

    // Specular strength of the material (values in the range [0, 1])
    float specularStrength = 0.5f;

    // Diffuse strength of the material (values in the range [0, 1])
    float diffuseStrength = 1.0f;

    // Albedo texture index of the material
    std::optional<unsigned int> albedoTextureIndex = {};

    // Normal texture index of the material
    std::optional<unsigned int> normalTextureIndex = {};

    // Constructor
    SceneMaterial() = default;

    SceneMaterial(const glm::vec3 &color, float shininess, float specularStrength, float diffuseStrength,
                  const optional<unsigned int> &albedoTextureIndex, const optional<unsigned int> &normalTextureIndex);

    // Default destructor
    ~SceneMaterial() = default;
};

// Represent a object in the scene
struct ObjectDescription {
public:
    // Transformation order translate-rotate-scale
    // Position of the object (meters)
    glm::vec3 position = glm::vec3(0, 0, 0);

    // Rotation of the object (radians). Order rotation in x, rotation in y, rotation in z
    glm::vec3 rotation = glm::vec3(0, 0, 0);

    // Scale of the object
    glm::vec3 scale = glm::vec3(0, 0, 0);

    // Material index of the object
    unsigned int materialIndex = 0;

    // Mesh index of the object
    unsigned int meshIndex = 0;

    // Constructor
    ObjectDescription() = default;

    ObjectDescription(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale,
                      unsigned int materialIndex, unsigned int meshIndex);

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

    // Materials in the scene
    vector<SceneMaterial> materials;

    // Objects in the scene
    vector<ObjectDescription> objects;

    // Light in the scene (only one from now)
    SceneLight light;

    // Ambient light in the scene
    AmbientLight ambientLight;

    // Camera in the scene
    SceneCamera camera;

    // Constructor
    SceneDescription(vector<SceneMesh> meshes, vector<PNG> textures,
                     vector<SceneMaterial> materials, vector<ObjectDescription> objects,
                     const SceneLight &light, const AmbientLight &ambientLight, const SceneCamera &camera);

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
