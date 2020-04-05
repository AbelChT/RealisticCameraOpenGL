#version 420 core
layout(binding=0) uniform sampler2D tex;
layout(binding=1) uniform sampler2D env;
uniform float kd;
uniform vec3 dcol;
uniform float ks;
uniform vec3 scol;
uniform float ns;
uniform bool texflg;
uniform bool envflg;
in SDATA
{
    vec3 N;
    vec3 L;
    vec3 V;
    vec3 tc;
} sdata;
out vec3 color;
void main()
{
    const float pi=3.14159265358979323846;
    vec3 N = normalize(sdata.N);
    vec3 L = normalize(sdata.L);
    vec3 V = normalize(sdata.V);
    color = dcol*vec3(0.1);
    float NL = max(dot(N,L),0);
    if (NL>0)
    {
        vec3 rd = dcol;
        if (texflg)
        rd = texture(tex,sdata.tc.st).rgb;
        color += kd*rd*NL;
        vec3 rs = vec3(0);
        vec3 R = reflect(-L,N);
        float RV = max(dot(R,V),0);
        if (RV>0)
        {
            rs = scol*pow(RV,ns);
            color += ks*rs*NL;
        }
        if (envflg)
        {
            vec3 M = reflect(-V,N);
            vec2 ec;
            float l = length(M.xy);
            ec.s = 0.5 + (1/(2*pi))*atan(-M.y,M.x);
            ec.t = 0.5 + (1/(  pi))*atan(-M.z,l);
            color += ks*texture(env,ec).rgb;
        }
    }
}