#version 420 core

out vec4 color;
in vec2 TexCoords;

uniform sampler2D screenTexture0;
uniform sampler2D screenTexture1;

void main()
{
    vec3 col1 = texture(screenTexture0, TexCoords).rgb;
    vec3 col2 = texture(screenTexture1, TexCoords).rgb;
    vec3 finalCol = (col1 * 0.5) + (col2*0.5);
    color = vec4(finalCol, 1.0);
}