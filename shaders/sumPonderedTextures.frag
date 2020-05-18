#version 430 core

in vec2 textureCoordinates;
uniform sampler2DArray textureArray;
uniform int numberOfFrames;
uniform float weighingOfFirstFrame;

out vec4 color;

void main()
{
    float multiplier = (1.0 - weighingOfFirstFrame) / float(numberOfFrames);
    vec3 finalCol = weighingOfFirstFrame * texture(textureArray, vec3(textureCoordinates, 0)).rgb;
    for (int i = 1; i < numberOfFrames; i++){
        vec3 col = texture(textureArray, vec3(textureCoordinates, i)).rgb;
        finalCol = finalCol + (col * multiplier);
    }
    color = vec4(finalCol, 1.0);
}