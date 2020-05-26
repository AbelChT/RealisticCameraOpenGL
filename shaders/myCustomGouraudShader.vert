#version 420 core
layout(location=0) in vec3 vertexPosition;
layout(location=1) in vec3 vertexNormal;
layout(location=2) in vec3 vertexTextureLocation;
uniform mat4 worldClipMatrix;
uniform mat4 modelWorldMatrix;
uniform vec3 pinholePosition;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 ambientLightComponent;
uniform mat3 normalMatrix;
uniform float objectShininess;
uniform float objectSpecularStrength;
uniform float objectDiffuseStrength;

out SDATA
{
    vec3 color;
    vec3 tc;
} sdata;
void main()
{
    // Obtain the vertex in world coordinates
    vec4 vertexPositionWorld = modelWorldMatrix * vec4(vertexPosition, 1);

    // Obtain the light direction
    vec3 lightDirection = normalize(lightPosition - vertexPositionWorld.xyz);

    // Obtain the normal in world cordinates
    vec3 vertexNormalWorld = normalMatrix * vertexNormal;

    // Obtain the difuse component
    float diffuseImpact = max(dot(vertexNormalWorld, lightDirection), 0.0);
    vec3 diffuseComponent = (objectDiffuseStrength * diffuseImpact) * lightColor;

    // Obtain the specular component
    vec3 viewDirection = normalize(pinholePosition - vertexPositionWorld.xyz);
    vec3 reflectDirection = reflect(-lightDirection, vertexNormalWorld);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0), objectShininess);
    vec3 specularComponent = objectSpecularStrength * specularImpact * lightColor;

    // Pass data to next stage
    sdata.color = (ambientLightComponent + diffuseComponent + specularComponent) * objectColor;
    sdata.tc = vertexTextureLocation;
    gl_Position = worldClipMatrix * vertexPositionWorld;
}