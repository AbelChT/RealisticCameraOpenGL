#pragma once

#include "SceneDescription.h"

#include <string>
#include <optional>

namespace scene_reader
{
    std::optional<scene_description::SceneDescription> readScene(const std::string &sceneName);
}
