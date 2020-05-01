#version 420 core
// layout(binding=0) uniform sampler2D tex;
// uniform bool texflg;
// uniform bool envflg;
in SDATA
{
    vec3 color;
//    vec3 tc;
} sdata;
out vec4 color;
void main()
{
    color = vec4(sdata.color, 1.0);
//    if (texflg)
    //    color = texture(tex, sdata.tc.st);
}