#include "obj.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>
#include <glm/gtx/normal.hpp>

using namespace std;

OBJ::OBJ()
{
}

OBJ::OBJ(const string& fname,const glm::mat4& xf)
{
	load(fname,xf);
}

OBJ::~OBJ()
{
}

bool OBJ::load(const string& fname,const glm::mat4& xf)
{
	v.clear();
	vn.clear();
	vt.clear();
	fv.clear();
	fvn.clear();
	fvt.clear();

	ifstream is;

	is.open(fname);
	if (!is)
		return false;

	string tag;

	while (is>>tag)
	{
		if (tag[0]=='#')
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}

		// Vertex data
		if (tag=="v")
		{
			load_v(is);
			continue;
		}
		if (tag=="vn")
		{
			load_vn(is);
			continue;
		}
		if (tag=="vt")
		{
			load_vt(is);
			continue;
		}

		// Elements
		if (tag=="p")
		{
			// Point
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="l")
		{
			// Line
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="curv")
		{
			// Curve
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="curv2")
		{
			// Curve
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="f")
		{
			// Face
			load_f(is);
			continue;
		}
		if (tag=="surf")
		{
			// Surface
			is.ignore(0xFFFF,'\n');
			continue;
		}

		// Grouping
		if (tag=="o")
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="g")
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="s")
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="mg")
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}

		// Attributes
		if (tag=="mtllib")
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}
		if (tag=="usemtl")
		{
			is.ignore(0xFFFF,'\n');
			continue;
		}
		is.ignore(0xFFFF,'\n');
	}

	is.close();

	v.clear();
	vn.clear();

	normalize();

	if (fvn.size()==0)
		make_normals();
	if (fvt.size()==0)
		make_texcoord();

	for (auto& v : fv)
		v = glm::vec3(xf*glm::vec4(v.x,v.y,v.z,1.0f));
	for (auto& n : fvn)
		n = glm::mat3(xf)*n;

	return true;
}

void OBJ::load_v(ifstream& is)
{
	is >> ws;
	string params;
	getline(is,params);
	stringstream ss(params);

	float x=0.0,y=0.0,z=0.0,w=0.0;
	ss >> x >> y >> z >> w;

	v.push_back(glm::vec3(x,y,z));
}

void OBJ::load_vn(ifstream& is)
{
	is >> ws;
	string params;
	getline(is,params);
	stringstream ss(params);

	float x=0.0,y=0.0,z=0.0;
	ss >> x >> y >> z;

	vn.push_back(glm::vec3(x,y,z));
}

void OBJ::load_vt(ifstream& is)
{
	is >> ws;
	string params;
	getline(is,params);
	stringstream ss(params);

	float x=0.0,y=0.0,z=0.0;
	ss >> x >> y >> z;

	vt.push_back(glm::vec3(x,y,z));
}

void OBJ::load_f(ifstream& is)
{
	is >> ws;
	string params;
	getline(is,params);
	stringstream ss(params);

	vector<int> ifv;
	vector<int> ifvn;
	vector<int> ifvt;

	string vs;
	while (ss >> vs)
	{
		stringstream vss(vs);

		int nsl = count(vs.begin(),vs.end(),'/');

		int kv  = 0;
		int kvt = 0;
		int kvn = 0;
		switch (nsl)
		{
			case 0:
				vss >> kv;
				break;
			case 1:
				vss >> kv;
				vss.ignore(0xFFFF,'/');
				vss >> kvt;
				break;
			case 2:
				vss >> kv;
				vss.ignore(0xFFFF,'/');
				if (vss.peek()!='/')
					vss >> kvt;
				vss.ignore(0xFFFF,'/');
				vss >> kvn;
				break;
		}

		if (kv<=0) continue;
		ifv.push_back(kv);

		if (kvt>0)
			ifvt.push_back(kvt);
		if (kvn>0)
			ifvn.push_back(kvn);
	}

	bool hasn = (ifvn.size()>0);
	bool hast = (ifvt.size()>0);
	for (int i=1; i<(ifv.size()-1); i++)
	{
		fv.push_back(v[ifv[0  ]-1]);
		fv.push_back(v[ifv[i  ]-1]);
		fv.push_back(v[ifv[i+1]-1]);
		if (hasn)
		{
		fvn.push_back(vn[ifvn[0  ]-1]);
		fvn.push_back(vn[ifvn[i  ]-1]);
		fvn.push_back(vn[ifvn[i+1]-1]);
		}
		if (hast)
		{
		fvt.push_back(vt[ifvt[0  ]-1]);
		fvt.push_back(vt[ifvt[i  ]-1]);
		fvt.push_back(vt[ifvt[i+1]-1]);
		}
	}
}

void OBJ::normalize()
{
	if (fv.size()==0)
		return;

	glm::vec3 minv(fv[0]),maxv(fv[0]);
	for (const auto v : fv)
	{
		if (v.x<minv.x) minv.x = v.x;
		if (v.y<minv.y) minv.y = v.y;
		if (v.z<minv.z) minv.z = v.z;
		if (v.x>maxv.x) maxv.x = v.x;
		if (v.y>maxv.y) maxv.y = v.y;
		if (v.z>maxv.z) maxv.z = v.z;
	}

	glm::vec3 size = maxv - minv;
//	cout << "sz " << size.x << " " << size.y << " " << size.z << endl;
	glm::vec3 center = (maxv + minv);
	center.x *= 0.5;
	center.y *= 0.5;
	center.z *= 0.5;
	float k = max(size.x,max(size.y,size.z));
	for (auto& v : fv)
	{
		v.x -= center.x;
		v.y -= center.y;
		v.z -= center.z;
		v.x /= k;
		v.y /= k;
		v.z /= k;
	}
}

void OBJ::make_normals()
{
	for (int i=0; i<fv.size(); i+=3)
	{
		glm::vec3 n = glm::triangleNormal(fv[i],fv[i+1],fv[i+2]);
		fvn.push_back(n);
		fvn.push_back(n);
		fvn.push_back(n);
	}
}

void OBJ::make_texcoord()
{
	for (int i=0; i<fv.size(); i+=3)
	{
		glm::vec3 t = fv[i];
		fvt.push_back(t);
		fvt.push_back(t);
		fvt.push_back(t);
	}
}
