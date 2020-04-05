#version 420 core
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vnor;
layout(location=2) in vec3 vtc;
uniform mat4 view;
uniform vec3 eye;
uniform vec3 light;
out SDATA
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec3 tc;
} sdata;
void main()
{
    sdata.N = vnor;
    sdata.L = normalize(light-vpos);
    sdata.V = normalize(eye-vpos);
    sdata.tc = vtc;
    gl_Position = view*vec4(vpos,1.0);
}