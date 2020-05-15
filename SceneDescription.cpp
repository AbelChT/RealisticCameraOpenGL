//
// Created by abel on 15/5/20.
//
#include "SceneDescription.h"

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
    float rotationRadius = (cameraDefinition.focalLength / cameraDefinition.fStop) / 1000;
    return SceneCamera(position, lookAt, fieldOfView, zNear, zFar, rotationRadius);
}

SceneMesh::SceneMesh(vector<glm::vec3> vertices, vector<glm::vec3> normals,
                     vector<glm::vec3> texturePositions) : vertices(std::move(vertices)), normals(std::move(normals)),
                                                           texture_positions(std::move(texturePositions)) {}

SceneCamera::SceneCamera() : position(glm::vec3(0, 0, 0)), lookAt(glm::vec3(0, 0, 0)), fieldOfView(0), zNear(0),
                             zFar(0), rotationRadius(0) {}

SceneCamera::SceneCamera(const glm::vec3 &position, const glm::vec3 &lookAt, float fieldOfView, float zNear, float zFar,
                         float rotationRadius)
        : position(position), lookAt(lookAt), fieldOfView(fieldOfView), zNear(zNear), zFar(zFar),
          rotationRadius(rotationRadius) {}

CameraDefinition::CameraDefinition(const glm::vec3 &position, const glm::vec3 &lookAt, float sensorSize,
                                   float focalLength, float fStop, float zFar) : position(position), lookAt(lookAt),
                                                                                 sensorSize(sensorSize),
                                                                                 focalLength(focalLength), fStop(fStop),
                                                                                 zFar(zFar) {}

SceneLight::SceneLight(const glm::vec3 &position) : position(position) {}

ObjectDescription::ObjectDescription(const glm::vec3 &position, const glm::vec3 &rotation, const glm::vec3 &scale,
                                     const glm::ivec3 &color, bool useColor, bool useTexture,
                                     unsigned int meshIndex) : position(position), rotation(rotation), scale(scale),
                                                               color(color),
                                                               useColor(useColor), useTexture(useTexture),
                                                               meshIndex(meshIndex) {}

SceneDescription::SceneDescription(vector<SceneMesh> meshes, PNG texture, vector<ObjectDescription> objects,
                                   const SceneLight &light, const SceneCamera &camera) : meshes(std::move(meshes)),
                                                                                         texture(std::move(texture)),
                                                                                         objects(std::move(objects)),
                                                                                         light(light),
                                                                                         camera(camera) {}