#version 420 core
uniform sampler2D tex;

// Texture or color controller
uniform bool enableTextureFlag;

in SDATA
{
    vec3 color;
    vec3 tc;
} sdata;
out vec4 color;
void main()
{
    if (enableTextureFlag){
        color = texture(tex, sdata.tc.st);
    } else {
        color = vec4(sdata.color, 1.0);
    }
}