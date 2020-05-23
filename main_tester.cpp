#include <iostream>
#include "SceneReader.h"

//
// Created by abel on 22/5/20.
//
int main(int argc, char *argv[]) {
    auto returned_data = readScene("assets/scene/scene1.json");
    if(returned_data.has_value())
        std::cout << "Has value" << std::endl;
    else
        std::cout << "Don't has value" << std::endl;
    return 0;
}
