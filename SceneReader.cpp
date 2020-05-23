//
// Created by abel on 22/5/20.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include <optional>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>

#include "SceneReader.h"
#include "libs/obj.h"

class SceneReadingException : public std::exception {
private:
    std::string s;

public:
    explicit SceneReadingException(std::string ss) : s(std::move(ss)) {}

    ~SceneReadingException() noexcept override = default;

    [[nodiscard]] const char *what() const noexcept override { return s.c_str(); }
};

using json = nlohmann::json;

//SceneDescription readScene(std::string sceneName){
std::optional<SceneDescription> readScene(const std::string &scenePath) {
    // read a JSON file
    std::ifstream inputJson(scenePath);
    json jsonDeserialized;
    inputJson >> jsonDeserialized;

    try {
        // Load textures
        auto textures = jsonDeserialized["textures"];
        // Store the textures
        std::vector<PNG> textures_vector(textures.size());
        // Texture position name association
        std::map<std::string, unsigned int> texture_position_name_association;

        for (auto &i: textures) {
            auto textureName = i["name"].get<std::string>();
            auto texturePath = i["path"].get<std::string>();

            // Check if texture name is duplicated
            if (texture_position_name_association.contains(textureName))
                throw SceneReadingException("Duplicated texture name: " + textureName);

            // Id of the texture
            unsigned int texturePositionInVector = texture_position_name_association.size();

            // Save texture
            textures_vector[texturePositionInVector] = PNG(texturePath);

            // Check if texture have been correctly loaded
            if (textures_vector[texturePositionInVector].width() == 0 ||
                textures_vector[texturePositionInVector].height() == 0)
                throw SceneReadingException("Error loading texture: " + textureName);

            // Save texture name id association
            texture_position_name_association[textureName] = texturePositionInVector;
        }

        // Load meshes
        auto meshes = jsonDeserialized["meshes"];
        // Store the meshes
        std::vector<SceneMesh> meshes_vector(meshes.size());
        // Mesh position name association
        std::map<std::string, unsigned int> meshes_position_name_association;

        for (auto &i: meshes) {
            auto meshName = i["name"].get<std::string>();
            auto meshPath = i["path"].get<std::string>();
            auto rotateMesh = i["rotate"].get<bool>();
            auto uniformMesh = i["uniform"].get<bool>();

            // Check if mesh name is duplicated
            if (meshes_position_name_association.contains(meshName))
                throw SceneReadingException("Duplicated mesh name: " + meshName);

            // Id of the mesh
            unsigned int meshPositionInVector = meshes_position_name_association.size();

            // Meshes definition
            glm::mat4 xf = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

            OBJ obj;

            if (rotateMesh)
                obj.load(meshPath, xf, uniformMesh);
            else
                obj.load(meshPath, glm::mat4(1.0f), uniformMesh);

            // Check if mesh have been correctly loaded
            if (obj.faces().empty())
                throw SceneReadingException("Error loading mesh: " + meshName);

            meshes_vector[meshPositionInVector] = SceneMesh(obj.faces(), obj.normals(), obj.texcoord());

            // Save mesh name id association
            meshes_position_name_association[meshName] = meshPositionInVector;
        }

        // Load materials
        auto materials = jsonDeserialized["materials"];

        // Store the materials
        std::vector<SceneMaterial> materials_vector(materials.size());

        // Material position name association
        std::map<std::string, unsigned int> materials_position_name_association;

        for (auto &i: materials) {
            auto materialName = i["name"].get<std::string>();
            auto materialColor = i["color"];
            auto colorVector = glm::ivec3(materialColor["r"].get<int>(),
                                          materialColor["g"].get<int>(),
                                          materialColor["b"].get<int>());

            // Id of the material
            unsigned int materialPositionInVector = materials_position_name_association.size();

            // Scene material
            auto sceneMaterial = SceneMaterial(colorVector);

            // Save material
            materials_vector[materialPositionInVector] = sceneMaterial;

            // Save material name id association
            materials_position_name_association[materialName] = materialPositionInVector;
        }

        // Load objects
        auto objects = jsonDeserialized["objects"];

        // Store the objects
        std::vector<ObjectDescription> objects_vector(objects.size());

        int objects_index = 0;
        for (auto &i: objects) {
            // Obtain index of the mesh
            auto meshName = i["mesh_name"].get<std::string>();
            if (!meshes_position_name_association.contains(meshName))
                throw SceneReadingException("Error loading object, non existent mesh: " + meshName);

            auto meshIndex = meshes_position_name_association[meshName];

            // Obtain index of the texture
            std::optional<unsigned int> textureIndex = {};

            if (i.contains("texture_name")) {
                auto textureName = i["texture_name"].get<std::string>();
                if (!texture_position_name_association.contains(textureName))
                    throw SceneReadingException("Error loading object, non existent texture: " + textureName);

                textureIndex = texture_position_name_association[textureName];
            }

            // Obtain index of the material
            std::optional<unsigned int> materialIndex = {};

            if (i.contains("material_name")) {
                auto materialName = i["material_name"].get<std::string>();
                if (!materials_position_name_association.contains(materialName))
                    throw SceneReadingException("Error loading object, non existent material: " + materialName);

                materialIndex = materials_position_name_association[materialName];
            }

            // Obtain position
            auto objectPosition = i["position"];
            auto positionVector = glm::vec3(objectPosition["x"].get<float>(),
                                            objectPosition["y"].get<float>(),
                                            objectPosition["z"].get<float>());
            // Obtain rotation
            auto objectRotation = i["rotation"];
            auto rotationVector = glm::vec3(objectRotation["x"].get<float>(),
                                            objectRotation["y"].get<float>(),
                                            objectRotation["z"].get<float>());

            // Obtain scale
            auto objectScale = i["scale"];
            auto scaleVector = glm::vec3(objectScale["x"].get<float>(),
                                         objectScale["y"].get<float>(),
                                         objectScale["z"].get<float>());


            // Create object description
            objects_vector[objects_index] = ObjectDescription(positionVector, rotationVector, scaleVector, textureIndex,
                                                              materialIndex, meshIndex);
            ++objects_index;
        }

        // Load light
        auto light = jsonDeserialized["light"];
        auto lightPosition = light["position"];
        auto lightPositionVector = glm::vec3(lightPosition["x"].get<float>(),
                                             lightPosition["y"].get<float>(),
                                             lightPosition["z"].get<float>());

        auto sceneLight = SceneLight(lightPositionVector);

        // Load camera

        int breakpoint = 0 ;
    }
    catch (json::exception &e) {
        // Bad input format
        std::cerr << "Bad input format" << std::endl;
    } catch (SceneReadingException &e) {
        // Custom exception
        std::cerr << e.what() << std::endl;
    }

    return {};

//    // Meshes definition
//    glm::mat4 xf = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
//
//    OBJ obj;
//    // obj.load("assets/teapot.obj", xf);
//    obj.load("assets/can.obj", glm::mat4(1.0f), false);
//
//    std::cout << "Faces " << obj.faces().size() << std::endl;
//
//    vector<SceneMesh> sceneMeshes{
//            SceneMesh(obj.faces(), obj.normals(), obj.texcoord())
//    };
//
//    // Textures definition
//    auto texture = PNG("tex/checker.png");
////    vector<PNG> textures{
////            PNG("tex/checker.png")
////    };
//
//    // Light definition
//    SceneLight sceneLight(glm::normalize(glm::vec3(1.0f)));
//
//    // Camera definition
////    glm::vec3 to(0, 0, 0);
////
////    float world_ph = 0.0;
////    float world_th = 30.0;
////
////    const float ph = glm::radians(world_ph);
////    const float th = glm::radians(world_th);
////
////    glm::vec3 axis(cos(ph) * cos(th), sin(ph) * cos(th), sin(th));
////    float cameraDistanceFromO = 4.0f;
////    glm::vec3 eye = to + cameraDistanceFromO * axis;
//
//    glm::vec3 eye(1.0, 0, 0.4);
//
//    float cameraRotationX = 80.0f;
//
//    glm::vec3 to(eye.x - sin(glm::radians(cameraRotationX)), 0, eye.z - cos(glm::radians(cameraRotationX)));
//
//    // TODO: Solve error with field of view
//    // SceneCamera sceneCamera(eye, to, 30.0f, 0.1f, 100.0f, 0.02);
//
//    CameraDefinition cameraDefinition(eye, to, 30, 32, 1,1000);
//
//    // Obtain camera from parameters
//    SceneCamera sceneCamera = cameraDefinitionToSceneCamera(cameraDefinition);
//    cout << "Field of view " << sceneCamera.fieldOfView << " rotation radius " << sceneCamera.rotationRadius << endl;
//
//    // Objects definition
//    vector<ObjectDescription> sceneObjects{
////            ObjectDescription(glm::vec3(0, 0, 0), glm::vec3(0, 0, 0),
////                              glm::vec3(1, 1, 1), glm::vec3(255, 30, 30),
////                              false, true, 0),
////            ObjectDescription(glm::vec3(-8, 2, 0), glm::vec3(0, 0, 0),
////                              glm::vec3(1, 1, 1), glm::vec3(30, 255, 30),
////                              false, true, 0),
////            ObjectDescription(glm::vec3(-32, -16, 0), glm::vec3(0, 0, 0),
////                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
////                              false, true, 0)
//            ObjectDescription(glm::vec3(0, -0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(205, 63, 33),
//                              false, true, 0),
//            ObjectDescription(glm::vec3(0, 0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//
//            ObjectDescription(glm::vec3(-0.2, -0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//            ObjectDescription(glm::vec3(-0.2, 0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//
//
//            ObjectDescription(glm::vec3(-0.4, -0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//            ObjectDescription(glm::vec3(-0.4, 0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//
//            ObjectDescription(glm::vec3(-0.6, -0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//            ObjectDescription(glm::vec3(-0.6, 0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//
//            ObjectDescription(glm::vec3(-0.8, -0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0),
//            ObjectDescription(glm::vec3(-0.8, 0.1, 0), glm::vec3(0, 0, 0),
//                              glm::vec3(1, 1, 1), glm::vec3(30, 30, 255),
//                              false, true, 0)
//    };
//
//    SceneDescription sceneDescription;
}