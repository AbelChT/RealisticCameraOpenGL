#version 420 core
uniform sampler2D tex;

// Texture or color controller
uniform bool enableTextureFlag;
uniform bool enableMaterialFlag;

uniform vec3 pinholePosition;
uniform vec3 lightPosition;

// I_efect
uniform vec3 lightColor;
uniform vec3 objectColor;
uniform vec3 ambientLightComponent;
// m
uniform float objectShininess;
// C_spec
uniform float objectSpecularStrength;
uniform float objectDiffuseStrength;

in SDATA
{
    vec3 normalVector;
    vec3 textureLocation;
    vec3 vertexPositionWorld;
} sdata;
out vec4 color;
void main()
{
    // Obtain the light direction
    vec3 lightDirection = normalize(lightPosition - sdata.vertexPositionWorld);

    // Obtain the view direction
    vec3 viewDirection = normalize(pinholePosition - sdata.vertexPositionWorld);

    // Direction vectors
    vec3 vertexNormalWorld = normalize(sdata.normalVector);

    // Obtain the difuse component
    float diffuseImpact = max(dot(vertexNormalWorld, lightDirection), 0.0);
    vec3 diffuseComponent = (objectDiffuseStrength * diffuseImpact) * lightColor;

    // Obtain the specular component
    vec3 reflectDirection = reflect(-lightDirection, vertexNormalWorld);
    float specularImpact = pow(max(dot(viewDirection, reflectDirection), 0.0), objectShininess);
    vec3 specularComponent = objectSpecularStrength * specularImpact * lightColor;

    // Calculate the color
    vec3 calculatedColor = (ambientLightComponent + diffuseComponent + specularComponent) * objectColor;

    if (enableTextureFlag && !enableMaterialFlag){
        color = texture(tex, sdata.textureLocation.st);
    } else if (!enableTextureFlag && enableMaterialFlag){
        color = vec4(calculatedColor, 1.0);
    } else {
        color = texture(tex, sdata.textureLocation.st) * 0.5 + vec4(calculatedColor, 1.0) * 0.5;
    }
}