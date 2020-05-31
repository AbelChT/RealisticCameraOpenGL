#version 420 core
layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec3 vertexTextureLocation;
uniform mat4 worldClipMatrix;
uniform mat4 modelWorldMatrix;
uniform mat3 normalMatrix;

out SDATA
{
    vec3 normalVector;
    vec3 textureLocation;
    vec3 vertexPositionWorld;
} sdata;

void main()
{
    // Obtain the vertex in world coordinates
    vec4 vertexPositionWorld = modelWorldMatrix * vec4(vertexPosition, 1);

    // Obtain the normal in world cordinates
    vec3 vertexNormalWorld = normalize(normalMatrix * vertexNormal);

    // Pass data to next stage
    sdata.normalVector = vertexNormalWorld;
    sdata.textureLocation = vertexTextureLocation;
    sdata.vertexPositionWorld = vertexPositionWorld.xyz;
    gl_Position = worldClipMatrix * vertexPositionWorld;
}