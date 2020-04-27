//
// Created by abel on 14/4/20.
//
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>
#include "../SceneDescription.h"

using namespace std;

void ProcessScene(const aiScene &scene, SceneDescription &sceneDescription) {
    // Get vertices
    for (int i = 0; i < scene.mNumMeshes; i++) {
        vector<glm::vec3> vertices(scene.mMeshes[i]->mNumVertices);
        vector<glm::vec3> normals(scene.mMeshes[i]->mNumVertices);

        // Load vertices data
        for (int j = 0; j < scene.mMeshes[i]->mNumVertices; ++j) {
            glm::vec3 vertex(scene.mMeshes[i]->mVertices->x, scene.mMeshes[i]->mVertices->y,
                             scene.mMeshes[i]->mVertices->z);
            vertices[j] = vertex;
        }

        // Load into the scene description
        sceneDescription.vertices.push_back(vertices);
        sceneDescription.normals.push_back(normals);
    }
}

bool importScene(const std::string &pFile, SceneDescription &sceneDescription) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene *scene = importer.ReadFile(pFile,
                                             aiProcess_CalcTangentSpace |
                                             aiProcess_Triangulate |
                                             aiProcess_JoinIdenticalVertices |
                                             aiProcess_SortByPType);

    // If the import failed, report it
    if (!scene) {
        cerr << importer.GetErrorString() << endl;
        return false;
    }

    // Now we can access the file's contents.
    ProcessScene(*scene, sceneDescription);


    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

int main() {
    SceneDescription sceneDescription;

    if (importScene("assets/threeObjectScene.dae", sceneDescription)) {
        for (auto &i:sceneDescription.vertices) {
            for (auto &j:i) {
                cout << "Vertex data: " << j.x << " " << j.y << " " << j.z << endl;
            }
        }

    }


    return 0;
}
