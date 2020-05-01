#version 420 core

layout (location = 0) in vec2 vertexPosition;
layout (location = 1) in vec2 texturePosition;

out vec2 textureCoordenates;

void main()
{
    gl_Position = vec4(vertexPosition.x, vertexPosition.y, 0.0, 1.0);
    textureCoordenates = texturePosition;
}