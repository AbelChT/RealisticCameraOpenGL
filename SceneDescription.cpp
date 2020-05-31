//
// Created by abel on 15/5/20.
//
#include "SceneDescription.h"

#include <utility>

#include <cmath>

/**
 * Transform a camera definition to a sceneCamera
 * @param cameraDefinition
 * @return
 */
SceneCamera cameraDefinitionToSceneCamera(CameraDefinition cameraDefinition) {
    glm::vec3 position = cameraDefinition.position;
    glm::vec3 lookAt = cameraDefinition.lookAt;
    float fieldOfView =
            (atan2(cameraDefinition.sensorSize, 2.0f * cameraDefinition.focalLength) * 360.0f) / (float) M_PI;
    float zNear = cameraDefinition.focalLength / 1000.0f;
    float zFar = cameraDefinition.zFar;
    float rotationRadius = (cameraDefinition.focalLength / cameraDefinition.fStop) / (1000 * 2);
    return SceneCamera(position, lookAt, fieldOfView, zNear, zFar, rotationRadius);
}

SceneMesh::SceneMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals,
                     vector<glm::vec3> texturePositions)
        : vertices(std::move(vertices)), normals(std::move(normals)), texture_positions(std::move(texturePositions)) {}

SceneCamera::SceneCamera(const glm::vec3 &position, const glm::vec3 &lookAt, float fieldOfView, float zNear, float zFar,
                         float rotationRadius)
        : position(position), lookAt(lookAt), fieldOfView(fieldOfView), zNear(zNear), zFar(zFar),
          rotationRadius(rotationRadius) {}

CameraDefinition::CameraDefinition(const glm::vec3 &position, const glm::vec3 &lookAt, float sensorSize,
                                   float focalLength, float fStop, float zFar)
        : position(position), lookAt(lookAt), sensorSize(sensorSize), focalLength(focalLength), fStop(fStop),
          zFar(zFar) {}

SceneLight::SceneLight(const glm::vec3 &position, const glm::vec3 &color) : position(position), color(color) {}

AmbientLight::AmbientLight(const glm::vec3 &color) : color(color) {}

SceneMaterial::SceneMaterial(const glm::vec3 &color, float shininess, float specularStrength, float diffuseStrength,
                             const optional<unsigned int> &albedoTextureIndex,
                             const optional<unsigned int> &normalTextureIndex) : color(color), shininess(shininess),
                                                                                 specularStrength(specularStrength),
                                                                                 diffuseStrength(diffuseStrength),
                                                                                 albedoTextureIndex(albedoTextureIndex),
                                                                                 normalTextureIndex(
                                                                                         normalTextureIndex) {}

ObjectDescription::ObjectDescription(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale,
                                     unsigned int materialIndex, unsigned int meshIndex) : position(position),
                                                                                           rotation(rotation),
                                                                                           scale(scale),
                                                                                           materialIndex(materialIndex),
                                                                                           meshIndex(meshIndex) {}

SceneDescription::SceneDescription(vector<SceneMesh> meshes, vector<PNG> textures,
                                   vector<SceneMaterial> materials, vector<ObjectDescription> objects,
                                   const SceneLight &light, const AmbientLight &ambientLight, const SceneCamera &camera)
        : meshes(std::move(meshes)), textures(std::move(textures)), materials(std::move(materials)), objects(std::move(objects)), light(light),
          ambientLight(ambientLight), camera(camera) {}
