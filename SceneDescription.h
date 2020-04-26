//
// Created by abel on 26/4/20.
//

#ifndef TESTINGOPENGL_SCENEDESCRIPTION_H
#define TESTINGOPENGL_SCENEDESCRIPTION_H

#include <vector>
#include <list>
#include <glm/vec3.hpp>

using namespace std;

struct SceneDescription {
public:
    // Vertices of the scene by mesh
    list<vector<glm::vec3>> vertices;

    // Normals of the scene by mesh
    list<vector<glm::vec3>> normals;
};


#endif //TESTINGOPENGL_SCENEDESCRIPTION_H
