//
// Created by abel on 27/4/20.
//

#ifndef TESTINGOPENGL_SCENERENDERER_H
#define TESTINGOPENGL_SCENERENDERER_H

#include "SceneDescription.h"

void initSceneRenderer(const SceneDescription &sceneDescription);

void reshapeScene(int w, int h);

void renderFrame(int w, int h, bool withFieldOfView);

#endif //TESTINGOPENGL_SCENERENDERER_H
