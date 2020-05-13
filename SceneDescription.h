//
// Created by abel on 26/4/20.
//

#ifndef TESTINGOPENGL_SCENEDESCRIPTION_H
#define TESTINGOPENGL_SCENEDESCRIPTION_H

#include <utility>
#include <vector>
#include <list>
#include <glm/vec3.hpp>
#include "base_code/png.h"

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
              vector<glm::vec3> texturePositions) : vertices(std::move(vertices)), normals(std::move(normals)),
                                                    texture_positions(std::move(texturePositions)) {}
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

    SceneCamera(const glm::vec3 &position, const glm::vec3 &lookAt, float fieldOfView, float zNear, float zFar)
            : position(position), lookAt(lookAt), fieldOfView(fieldOfView), zNear(zNear), zFar(zFar) {}
};

// Parameters that define a camera in the real world
struct CameraDefinition {
public:
    // Position of the scene camera
    glm::vec3 position;

    // Look at
    glm::vec3 lookAt;

    // Large of the film
    float filmLarge;

    // Distance to the film
    float filmDistance;

    // f-stop
    float fStop;

    // TODO: Should have a function to obtain a SceneCamera
    // Constructor
    CameraDefinition(const glm::vec3 &position, const glm::vec3 &lookAt, float filmLarge, float filmDistance,
                     float fStop)
            : position(position), lookAt(lookAt), filmLarge(filmLarge), filmDistance(filmDistance), fStop(fStop) {}
};

// Represent a light in the scene
struct SceneLight {
public:
    // Position of the scene light
    glm::vec3 position;

    // Constructor
    explicit SceneLight(const glm::vec3 &position) : position(position) {}
};

struct ObjectDescription {
public:
    // Transformation order translate-rotate-scale
    // Position of the object
    glm::vec3 position;

    // Rotation of the object
    glm::vec3 rotation;

    // Scale of the object
    glm::vec3 scale;

    // Color of the object
    glm::ivec3 color;

    // Use color
    bool useColor;

    // Texture index of the object
    unsigned int textureIndex;

    // Use texture
    bool useTexture;

    // Mesh index of the object
    unsigned int meshIndex;

    // Constructor
    ObjectDescription(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale,
                      const glm::ivec3 &color, bool useColor, unsigned int textureIndex, bool useTexture,
                      unsigned int meshIndex) : position(position), rotation(rotation), scale(scale), color(color),
                                                useColor(useColor), textureIndex(textureIndex), useTexture(useTexture),
                                                meshIndex(meshIndex) {}
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

    // Light in the scene
    SceneLight light;

    // Camera in the scene
    SceneCamera camera;

    // Constructor
    SceneDescription(vector<SceneMesh> meshes, vector<PNG> textures,
                     vector<ObjectDescription> objects, const SceneLight &light, const SceneCamera &camera)
            : meshes(std::move(meshes)), textures(std::move(textures)), objects(std::move(objects)), light(light),
              camera(camera) {}
};


#endif //TESTINGOPENGL_SCENEDESCRIPTION_H
