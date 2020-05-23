//
// Created by abel on 22/5/20.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <map>
#include <utility>
#include "SceneReader.h"

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
void readScene(const std::string &sceneName) {
    // read a JSON file
    std::ifstream inputJson(sceneName);
    json jsonDeserialized;
    inputJson >> jsonDeserialized;

    try {
        // Load textures
        auto textures = jsonDeserialized["textures"];
        // Store the textures
        vector<PNG> textures_vector(textures.size());
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

    }
    catch (json::exception &e) {
        // Bad input format
        std::cerr << "Bad input format" << std::endl;
    } catch (SceneReadingException &e) {
        // Custom exception
        std::cerr << e.what() << std::endl;
    }


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