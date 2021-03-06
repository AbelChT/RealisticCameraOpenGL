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
            auto colorVector = glm::vec3(materialColor["r"].get<float>(),
                                          materialColor["g"].get<float>(),
                                          materialColor["b"].get<float>());

            // Obtain index of the albedo texture
            std::optional<unsigned int> albedoTextureIndex = {};

            if (i.contains("albedo_texture_name")) {
                auto textureName = i["albedo_texture_name"].get<std::string>();
                if (!texture_position_name_association.contains(textureName))
                    throw SceneReadingException("Error loading material, non existent texture: " + textureName);

                albedoTextureIndex = texture_position_name_association[textureName];
            }

            // Obtain index of the normal texture
            std::optional<unsigned int> normalTextureIndex = {};

            if (i.contains("normal_texture_name")) {
                auto textureName = i["normal_texture_name"].get<std::string>();
                if (!texture_position_name_association.contains(textureName))
                    throw SceneReadingException("Error loading material, non existent texture: " + textureName);

                normalTextureIndex = texture_position_name_association[textureName];
            }

            // Obtain light behaviour components
            auto shininess = i["shininess"].get<float>();
            auto specularStrength = i["specular_strength"].get<float>();
            auto diffuseStrength = i["diffuse_strength"].get<float>();


            // Id of the material
            unsigned int materialPositionInVector = materials_position_name_association.size();

            // Scene material
            auto sceneMaterial = SceneMaterial(colorVector, shininess, specularStrength, diffuseStrength,
                                               albedoTextureIndex, normalTextureIndex);

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



            // Obtain index of the material
            unsigned int materialIndex;

            if (i.contains("material_name")) {
                auto materialName = i["material_name"].get<std::string>();
                if (!materials_position_name_association.contains(materialName))
                    throw SceneReadingException("Error loading object, non existent material: " + materialName);

                materialIndex = materials_position_name_association[materialName];
            } else {
                throw SceneReadingException("Error loading object, material not provided");
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
            objects_vector[objects_index] = ObjectDescription(positionVector, rotationVector, scaleVector,
                                                              materialIndex, meshIndex);
            ++objects_index;
        }

        // Load light
        auto sceneLightJson = jsonDeserialized["light"];
        auto sceneLightPosition = sceneLightJson["position"];
        auto sceneLightPositionVector = glm::vec3(sceneLightPosition["x"].get<float>(),
                                                  sceneLightPosition["y"].get<float>(),
                                                  sceneLightPosition["z"].get<float>());
        auto sceneLightColor = sceneLightJson["color"];
        auto sceneLightColorVector = glm::vec3(sceneLightColor["r"].get<float>(),
                                                sceneLightColor["g"].get<float>(),
                                                sceneLightColor["b"].get<float>());

        auto sceneLight = SceneLight(sceneLightPositionVector, sceneLightColorVector);

        // Load ambient light
        auto ambientLightJson = jsonDeserialized["ambient_light"];

        auto ambientLightColor = ambientLightJson["color"];
        auto ambientLightColorVector = glm::vec3(ambientLightColor["r"].get<float>(),
                                                  ambientLightColor["g"].get<float>(),
                                                  ambientLightColor["b"].get<float>());

        auto ambientLight = AmbientLight(ambientLightColorVector);

        // Load camera
        auto camera = jsonDeserialized["camera"];
        auto cameraPosition = camera["position"];
        auto cameraPositionVector = glm::vec3(cameraPosition["x"].get<float>(),
                                              cameraPosition["y"].get<float>(),
                                              cameraPosition["z"].get<float>());

        auto cameraLookAt = camera["look_at"];
        auto cameraLookAtVector = glm::vec3(cameraLookAt["x"].get<float>(),
                                            cameraLookAt["y"].get<float>(),
                                            cameraLookAt["z"].get<float>());

        auto cameraSensorSize = camera["sensor_size"].get<float>();

        auto cameraFocalLength = camera["focal_length"].get<float>();

        auto cameraFStop = camera["f_stop"].get<float>();

        auto cameraZFar = camera["z_far"].get<float>();

        auto cameraDefinition = CameraDefinition(cameraPositionVector, cameraLookAtVector, cameraSensorSize,
                                                 cameraFocalLength, cameraFStop, cameraZFar);

        auto sceneCamera = cameraDefinitionToSceneCamera(cameraDefinition);

        return SceneDescription(meshes_vector, textures_vector, materials_vector, objects_vector, sceneLight,
                                ambientLight, sceneCamera);
    }
    catch (json::exception &e) {
        // Bad input format
        std::cerr << "Bad input format" << std::endl;
        return {};
    } catch (SceneReadingException &e) {
        // Custom exception
        std::cerr << e.what() << std::endl;
        return {};
    }
}