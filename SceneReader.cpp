//
// Created by abel on 22/5/20.
//

#include <fstream>
#include <nlohmann/json.hpp>
#include <iostream>
#include <string>
#include <map>
#include "SceneReader.h"

using json = nlohmann::json;

//SceneDescription readScene(std::string sceneName){
void readScene(std::string sceneName) {
    // read a JSON file
    std::ifstream inputJson(sceneName);
    json jsonDeserialized;
    inputJson >> jsonDeserialized;

    try {
        std::map<std::string, int> m;

        // Load textures
        auto textures = jsonDeserialized["textures"];
        for (auto &i: textures) {
            auto textureName = i["name"].get<std::string>();
            auto texturePath = i["path"].get<std::string>();
            // TODO: Save values in the has
        }
    }
    catch (json::exception &e) {
        // output exception information
        std::cout << "message: " << e.what() << '\n'
                  << "exception id: " << e.id << std::endl;
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