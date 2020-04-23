//
// Created by abel on 14/4/20.
//
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <iostream>

using namespace std;

bool DoTheImportThing( const std::string& pFile) {
    // Create an instance of the Importer class
    Assimp::Importer importer;

    // And have it read the given file with some example postprocessing
    // Usually - if speed is not the most important aspect for you - you'll
    // probably to request more postprocessing than we do in this example.
    const aiScene* scene = importer.ReadFile( pFile,
                                              aiProcess_CalcTangentSpace       |
                                              aiProcess_Triangulate            |
                                              aiProcess_JoinIdenticalVertices  |
                                              aiProcess_SortByPType);

    // If the import failed, report it
    if( !scene) {
        cerr << importer.GetErrorString() << endl;
        return false;
    }

    // Now we can access the file's contents.
    // DoTheSceneProcessing( scene);

    // We're done. Everything will be cleaned up by the importer destructor
    return true;
}

int main(){
    DoTheImportThing("assets/cube.obj");
    return 0;
}
