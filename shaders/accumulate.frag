#version 420 core

out vec4 color;
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform sampler2D screenTexture2;

void main()
{
    vec3 col1 = texture(screenTexture, TexCoords).rgb;
    vec3 col2 = texture(screenTexture2, TexCoords).rgb;
    vec3 finalCol = (col1 * 0.5) + (col2*0.5);
    color = vec4(finalCol, 1.0);
}