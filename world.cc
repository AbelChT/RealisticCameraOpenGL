#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glu.h>

#define GLM_ENABLE_EXPERIMENTAL

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/polar_coordinates.hpp>

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include "world.h"
#include "obj.h"
#include "png.h"

using namespace std;

const char *gourd_vs_src = R"(
	#version 420 core
	layout(location=0) in vec3 vpos;
	layout(location=1) in vec3 vnor;
	layout(location=2) in vec3 vtc;
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
		vec3 tc;
	} sdata;
	void main()
	{
		vec3 N = normalize(vnor);
		vec3 L = normalize(light-vpos);
		vec3 V = normalize(eye-vpos);
		sdata.color = dcol*vec3(0.1);
		float NL = max(dot(N,L),0);
		if (NL>0)
		{
			sdata.color += kd*dcol*NL;
			vec3 R = reflect(-L,N);
			float RV = max(dot(R,V),0);
			if (RV>0)
				sdata.color += ks*scol*pow(RV,ns);
		}
		sdata.tc = vtc;
		gl_Position = view*vec4(vpos,1.0);
	}
)";
GLuint gourd_vs;

const char *gourd_fs_src = R"(
	#version 420 core
	layout(binding=0) uniform sampler2D tex;
	uniform bool texflg;
	uniform bool envflg;
	in SDATA
	{
		vec3 color;
		vec3 tc;
	} sdata;
	out vec4 color;
	void main()
	{
		color = vec4(sdata.color,1.0);
		if (texflg)
			color = texture(tex,sdata.tc.st);
	}
)";
GLuint gourd_fs;

GLuint gourd_prog;

const char *phong_vs_src = R"(
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
)";
GLuint phong_vs;

const char *phong_fs_src = R"(
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
)";
GLuint phong_fs;

GLuint phong_prog;

glm::mat4 view;
GLuint view_loc;
glm::vec3 light;
GLuint eye_loc;
GLuint light_loc;
GLuint kd_loc;
GLuint dcol_loc;
GLuint ks_loc;
GLuint scol_loc;
GLuint ns_loc;

bool world_fill = false;
bool world_pps = false;
bool world_tex = false;
GLuint texflg_loc;
bool world_env = false;
GLuint envflg_loc;

OBJ obj;
GLuint vao;
GLuint vao_sz;

PNG tex_png;
GLuint tex;
PNG env_png;
GLuint env;

void glcheck(const string &msg) {
    GLenum err;
    err = glGetError();
    if (err != GL_NO_ERROR)
        cout << msg << " error: " << gluErrorString(err) << endl;
}

void world_init() {
    glm::mat4 xf = glm::rotate(glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));

    obj.load("assets/cube.obj");
//	obj.load("../model/bb8.obj");
//	obj.load("../model/teapot.obj",xf);
//	obj.load("../model/dragon.obj",xf);
//	obj.load("../model/sphere.obj");
//	obj.load("../model/venus.obj",xf);
//	obj.load("../model/bunny.obj",xf);
//	obj.load("../model/armadillo.obj",xf);
//	obj.load("../model/tyra.obj",xf);
//	obj.load("../model/nefertiti.obj");

    cout << obj.faces().size() / 3 << endl;

    vao = 0;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    vao_sz = obj.faces().size();

    GLuint vpbo = 0;
    glGenBuffers(1, &vpbo);
    glBindBuffer(GL_ARRAY_BUFFER, vpbo);
    glBufferData(GL_ARRAY_BUFFER, vao_sz * sizeof(glm::vec3), obj.faces().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    GLuint vnbo = 0;
    glGenBuffers(1, &vnbo);
    glBindBuffer(GL_ARRAY_BUFFER, vnbo);
    glBufferData(GL_ARRAY_BUFFER, vao_sz * sizeof(glm::vec3), obj.normals().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    GLuint vtbo = 0;
    glGenBuffers(1, &vtbo);
    glBindBuffer(GL_ARRAY_BUFFER, vtbo);
    glBufferData(GL_ARRAY_BUFFER, vao_sz * sizeof(glm::vec3), obj.texcoord().data(), GL_STATIC_DRAW);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), NULL);

    phong_vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(phong_vs, 1, &phong_vs_src, NULL);
    glCompileShader(phong_vs);

    phong_fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(phong_fs, 1, &phong_fs_src, NULL);
    glCompileShader(phong_fs);

    phong_prog = glCreateProgram();
    glAttachShader(phong_prog, phong_vs);
    glAttachShader(phong_prog, phong_fs);
    glLinkProgram(phong_prog);

    gourd_vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(gourd_vs, 1, &gourd_vs_src, NULL);
    glCompileShader(gourd_vs);

    gourd_fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(gourd_fs, 1, &gourd_fs_src, NULL);
    glCompileShader(gourd_fs);

    gourd_prog = glCreateProgram();
    glAttachShader(gourd_prog, gourd_vs);
    glAttachShader(gourd_prog, gourd_fs);
    glLinkProgram(gourd_prog);

    glClearColor(0, 0, 0, 0);

//	tex_png.load("../tex/paper.png");
    tex_png.load("../tex/checker.png");
    glGenTextures(1, &tex);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);
    glTexStorage2D(GL_TEXTURE_2D,
                   8,
                   GL_RGB32F,
                   tex_png.width(), tex_png.height());
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0, 0,
                    tex_png.width(), tex_png.height(),
                    GL_RGB,
                    GL_FLOAT,
                    tex_png.pixels().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    env_png.load("../env/pano/uffizi.png");
//	env_png.load("../env/pano/city.png");
    glGenTextures(1, &env);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, env);
    glTexStorage2D(GL_TEXTURE_2D,
                   8,
                   GL_RGB32F,
                   env_png.width(), env_png.height());
    glTexSubImage2D(GL_TEXTURE_2D,
                    0,
                    0, 0,
                    env_png.width(), env_png.height(),
                    GL_RGB,
                    GL_FLOAT,
                    env_png.pixels().data());
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
}

void world_reshape(int w, int h) {
    glViewport(0, 0, w, h);
}


float world_ph = 0.0;
float world_th = 30.0;
float world_ro = 1.0;

void world_reset_cam() {
    world_ph = 0.0;
    world_th = 30.0;
    world_ro = 1.0;
}

void world_display(int w, int h) {
    if (h <= 0) return;
    if (w <= 0) return;

    float aspect = float(w) / float(h);

    glm::mat4 pers = glm::perspective(45.0f, aspect, 0.01f, 1000.0f);

    const float ph = glm::radians(world_ph);
    const float th = glm::radians(world_th);

    glm::vec3 axis(cos(ph) * cos(th), sin(ph) * cos(th), sin(th));

    glm::vec3 to(0, 0, 0);
    glm::vec3 eye = to + world_ro * axis;
    glm::mat4 camera = glm::lookAt(eye, to, glm::vec3(0, 0, 1));

    view = pers * camera;

//	light = axis;
    light = eye;
//	light = glm::normalize(glm::vec3(1.0f));
//	light = glm::normalize(glm::vec3(4.0f));

    glPolygonMode(GL_FRONT_AND_BACK, (world_fill ? GL_FILL : GL_LINE));
    glEnable(GL_DEPTH_TEST);
    glDisable(GL_CULL_FACE);

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    GLuint prg = (world_pps ? phong_prog : gourd_prog);
    glUseProgram(prg);
    eye_loc = glGetUniformLocation(prg, "eye");
    view_loc = glGetUniformLocation(prg, "view");
    light_loc = glGetUniformLocation(prg, "light");
    kd_loc = glGetUniformLocation(prg, "kd");
    dcol_loc = glGetUniformLocation(prg, "dcol");
    ks_loc = glGetUniformLocation(prg, "ks");
    scol_loc = glGetUniformLocation(prg, "scol");
    ns_loc = glGetUniformLocation(prg, "ns");
    texflg_loc = glGetUniformLocation(prg, "texflg");
    envflg_loc = glGetUniformLocation(prg, "envflg");

    glUniformMatrix4fv(view_loc, 1, GL_FALSE, glm::value_ptr(view));
    glUniform3fv(eye_loc, 1, glm::value_ptr(eye));
    glUniform3fv(light_loc, 1, glm::value_ptr(light));
    glUniform1f(kd_loc, 0.5f);
//	glUniform3f(dcol_loc,1.0,1.0,1.0);
    glUniform3f(dcol_loc, 212 / 255.0, 175 / 255.0, 55 / 255.0);
    glUniform1f(ks_loc, 0.5f);
    glUniform3f(scol_loc, 1.0, 1.0, 1.0);
//	glUniform3f(scol_loc,212/255.0,175/255.0,55/255.0);
    glUniform1f(ns_loc, 10.0f);
    glUniform1i(texflg_loc, world_tex);
    glUniform1i(envflg_loc, world_env);

    glBindVertexArray(vao);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);

    glDrawArrays(GL_TRIANGLES, 0, obj.faces().size());

//	glFlush();
}

void world_clean() {
}
