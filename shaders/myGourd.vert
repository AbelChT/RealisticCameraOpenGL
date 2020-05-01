#version 420 core
layout(location=0) in vec3 vpos;
layout(location=1) in vec3 vnor;
// layout(location=2) in vec3 vtc;
uniform mat4 view;
uniform vec3 eye;
uniform vec3 light;
uniform float kd;
uniform vec3 dcol;
uniform float ks;
uniform vec3 scol;
uniform float ns;

out SDATA
{
    vec3 color;
// vec3 tc;
} sdata;
void main()
{
    vec3 N = normalize(vnor);
    vec3 L = normalize(light-vpos);
    vec3 V = normalize(eye-vpos);
    sdata.color = dcol*vec3(0.1);
    float NL = max(dot(N, L), 0);
    if (NL>0)
    {
        sdata.color += kd*dcol*NL;
        vec3 R = reflect(-L, N);
        float RV = max(dot(R, V), 0);
        if (RV>0)
        sdata.color += ks*scol*pow(RV, ns);
    }
    // sdata.tc = vtc;
    gl_Position = view*vec4(vpos, 1.0);
}