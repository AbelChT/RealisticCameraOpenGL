//
// Created by abel on 27/4/20.
//

#ifndef TESTINGOPENGL_SCENERENDERER_H
#define TESTINGOPENGL_SCENERENDERER_H

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "SceneDescription.h"
#include "OpenglUtils.h"

void init_scene_renderer(const SceneDescription &sceneDescription);

void reshape_scene(int w, int h);

void render_frame(int w, int h);

#endif //TESTINGOPENGL_SCENERENDERER_H
