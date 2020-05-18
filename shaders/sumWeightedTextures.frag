#version 430 core

in vec2 textureCoordinates;
uniform sampler2DArray textureArray;
uniform int numberOfNewFrames;
uniform int numberOfAccumulatedFrames;

out vec4 color;

void main()
{
    float multiplier = 1.0 / float(numberOfNewFrames + numberOfAccumulatedFrames);
    float weighingOfFirstFrame = multiplier * float(numberOfAccumulatedFrames);
    vec3 finalCol = weighingOfFirstFrame * texture(textureArray, vec3(textureCoordinates, 0)).rgb;
    for (int i = 0; i < numberOfNewFrames; i++){
        vec3 col = texture(textureArray, vec3(textureCoordinates, i + 1)).rgb;
        finalCol = finalCol + (col * multiplier);
    }
    color = vec4(finalCol, 1.0);
}