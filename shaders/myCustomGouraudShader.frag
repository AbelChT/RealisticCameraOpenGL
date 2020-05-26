#version 420 core
uniform sampler2D tex;

// Texture or color controller
uniform bool enableTextureFlag;
uniform bool enableMaterialFlag;

in SDATA
{
    vec3 color;
    vec3 tc;
} sdata;
out vec4 color;
void main()
{
    if (enableTextureFlag && !enableMaterialFlag){
        color = texture(tex, sdata.tc.st);
    } else if (!enableTextureFlag && enableMaterialFlag){
        color = vec4(sdata.color, 1.0);
    } else {
        color = texture(tex, sdata.tc.st) * 0.5 + vec4(sdata.color, 1.0) * 0.5;
    }
}