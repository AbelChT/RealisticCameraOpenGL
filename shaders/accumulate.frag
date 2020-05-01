#version 420 core

out vec4 color;
in vec2 TexCoords;

uniform sampler2D screenTexture;

void main()
{
    vec3 col = texture(screenTexture, TexCoords).rgb;
    color = vec4(col, 1.0);
}