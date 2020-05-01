#version 420 core

in vec2 textureCoordenates;
uniform sampler2D texture0;
uniform sampler2D texture1;

out vec4 color;

void main()
{
    vec3 col1 = texture(texture0, textureCoordenates).rgb;
    vec3 col2 = texture(texture1, textureCoordenates).rgb;
    vec3 finalCol = (col1 * 0.5) + (col2*0.5);
    color = vec4(finalCol, 1.0);
}