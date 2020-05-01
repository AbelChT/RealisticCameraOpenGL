#version 430 core

in vec2 textureCoordenates;
uniform sampler2DArray textureArray;
uniform int numberOfFrames;

out vec4 color;

void main()
{
    float multiplier = 1 / float(numberOfFrames);
    vec3 finalCol = vec3(0.0, 0.0, 0.0);
    for (int i = 0; i < numberOfFrames; i++){
        vec3 col = texture(textureArray, vec3(textureCoordenates, i)).rgb;
        finalCol = finalCol + (col * multiplier);
    }
    color = vec4(finalCol, 1.0);
}