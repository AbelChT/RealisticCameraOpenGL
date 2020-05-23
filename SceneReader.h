//
// Created by abel on 22/5/20.
//

#ifndef TESTINGOPENGL_SCENEREADER_H
#define TESTINGOPENGL_SCENEREADER_H

#include "SceneDescription.h"
#include <string>
#include <optional>

std::optional<SceneDescription> readScene(const std::string &sceneName);

#endif //TESTINGOPENGL_SCENEREADER_H
