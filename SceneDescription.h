//
// Created by abel on 26/4/20.
//

#ifndef TESTINGOPENGL_SCENEDESCRIPTION_H
#define TESTINGOPENGL_SCENEDESCRIPTION_H

#include <vector>
#include <list>
#include <glm/vec3.hpp>

using namespace std;

// Represent an object in the scene
struct SceneMesh {
public:
    // Vertices of the scene
    vector<glm::vec3> vertices;

    // Normals of the scene
    vector<glm::vec3> normals;
};


// Represent a camera in the scene
struct SceneCamera {

};

// Represent a light in the scene
struct SceneLight {
public:
    glm::vec3 position;
};

// Description of the scene
struct SceneDescription {
public:
    // Meshes in the scene
    list<SceneMesh> meshes;

    // Lights in the scene
    list<SceneLight> lights;

    // Camera in the scene
    SceneCamera camera;
};


#endif //TESTINGOPENGL_SCENEDESCRIPTION_H
