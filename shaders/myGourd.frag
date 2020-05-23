#version 420 core
layout(binding=0) uniform sampler2D tex;

// Texture or color controller
uniform bool textureflg;
uniform bool colorflg;

in SDATA
{
    vec3 color;
    vec3 tc;
} sdata;
out vec4 color;
void main()
{
    if (textureflg && !colorflg){
        color = texture(tex, sdata.tc.st);
    } else if (!textureflg && colorflg){
        color = vec4(sdata.color, 1.0);
    } else {
        color = texture(tex, sdata.tc.st) * 0.5 + vec4(sdata.color, 1.0) * 0.5;
    }
}