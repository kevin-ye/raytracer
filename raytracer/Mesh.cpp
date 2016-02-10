#include <cstdio>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>
#include <fstream>

#include "cs488-framework/MathUtils.hpp"

// #include "cs488-framework/ObjFileDecoder.hpp"
#include "Mesh.hpp"
#include "Primitive.hpp"
#include "Tray.hpp"
#include "polyroots.hpp"
#include "IntersectedPrimitiveInfo.hpp"

using namespace std;
using namespace glm;

Mesh::Mesh( const string& fname )
	: m_vertices()
	, m_faces(),
	collisionMech_center(0, 0, 0),
	collisionMech_radius(0),
	bound(false)
{
	string code;
	double vx, vy, vz;
	bool flag_maxx, flag_maxy, flag_maxz;
	bool flag_minx, flag_miny, flag_minz;
	double maxx, maxy, maxz;
	double minx, miny, minz;
	flag_maxx = false;
	flag_maxy = false;
	flag_maxz = false;
	flag_minx = false;
	flag_miny = false;
	flag_minz = false;
	size_t s1, s2, s3;

	ifstream ifs( string("Assets/" + fname).c_str() );
	while( ifs >> code ) {
		if( code == "v" ) {
			ifs >> vx >> vy >> vz;
			m_vertices.push_back( vec3( vx, vy, vz ) );
			if ((!flag_maxx) || (maxx < vx))
			{
				flag_maxx = true;
				maxx = vx;
			}
			if ((!flag_maxy) || (maxy < vy))
			{
				flag_maxy = true;
				maxy = vy;
			}
			if ((!flag_maxz) || (maxz < vz))
			{
				flag_maxz = true;
				maxz = vz;
			}

			if ((!flag_minx) || (minx > vx))
			{
				flag_minx = true;
				minx = vx;
			}
			if ((!flag_miny) || (miny > vy))
			{
				flag_miny = true;
				miny = vy;
			}
			if ((!flag_minz) || (minz > vz))
			{
				flag_minz = true;
				minz = vz;
			}
		} else if( code == "f" ) {
			ifs >> s1 >> s2 >> s3;
			m_faces.push_back( Triangle( s1 - 1, s2 - 1, s3 - 1 ) );
		}
	}

	// center of all vetices
	collisionMech_center = vec3((maxx + minx) / 2, (maxy + miny) / 2, (maxz + minz) / 2);
	collisionMech_radius = maxdouble(-1, distance(collisionMech_center, vec3(maxx, maxy, maxz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(minx, maxy, maxz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(maxx, miny, maxz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(maxx, maxy, minz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(minx, miny, maxz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(minx, miny, minz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(minx, miny, maxz)));
	collisionMech_radius = maxdouble(collisionMech_radius, distance(collisionMech_center, vec3(minx, maxy, minz)));
}

ostream& operator<<(ostream& out, const Mesh& mesh)
{
  out << "mesh {";
  /*
  
  for( size_t idx = 0; idx < mesh.m_verts.size(); ++idx ) {
  	const MeshVertex& v = mesh.m_verts[idx];
  	out << to_string( v.m_position );
	if( mesh.m_have_norm ) {
  	  out << " / " << to_string( v.m_normal );
	}
	if( mesh.m_have_uv ) {
  	  out << " / " << to_string( v.m_uv );
	}
  }

*/
  out << "}";
  return out;
}

bool Mesh::intersect(Tray *theRay, vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info) {
	vec4 cinormalV;
	double ct;
	IntersectedPrimitiveInfo cinfo;
	bool collide = collisionDetection(theRay, cinormalV, ct, cinfo);
	if (!collide)
	{
		return false;
	} else if (bound) {
		if (info.SecondaryMode)
		{
			if ((!info.IsMesh) && (info.IntersectedPrimitive == this))
			{
				return false;
			}
		}
		inormalV = cinormalV;
		t = ct;
		info = cinfo;
		info.IsMesh = false;
		info.IntersectedPrimitive = this;
		return true;
	}
	bool flag_intersected = false;

	vec3 Ve = theRay->getEye();
	vec3 Vd = theRay->getVd();
	/*
	if (info.SecondaryMode)
	{
		// intersecting secondary ray
		if ((info.IsMesh) && (info.IntersectedPrimitive == this))
		{
			// skip the triangle where the surface point is
			return false;
		}
	}
	*/

	mat4 utrans = trans;

	for (unsigned int i = 0; i < m_faces.size(); ++i)
	{
		// current face
		if (info.SecondaryMode)
		{
			// intersecting secondary ray
			if ((info.IsMesh) && (info.MeshTriangleIndex == i))
			{
				// skip the triangle where the surface point is
				continue;
			}
		}
		vec3 v1 = m_vertices[m_faces[i].v1];
		vec3 v2 = m_vertices[m_faces[i].v2];
		vec3 v3 = m_vertices[m_faces[i].v3];
		vec3 normalV(0, 0, 0);
		// transform v1, v2, v3
		vec4 v1_4 = utrans * vec4(v1, 1);
		vec4 v2_4 = utrans * vec4(v2, 1);
		vec4 v3_4 = utrans * vec4(v3, 1);
		v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
		v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
		v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
		// choose x-y-basis on current face
		vec3 bx = (v2 - v1);
		vec3 by = (v3 - v1);
		vec3 be = (Ve - v1);
		normalV = normalize(cross(bx, by));
		// check if intersect is possible
		bool result = false;
		double newt = 0;
		if (dot(Vd, normalV) != 0)
		{
			mat3 matA = {
				-bx,
				-by,
				Vd
			};
			double detA = determinant(matA);
			// solve for t
			mat3 matt = {
				-bx,
				-by,
				-be
			};
			newt = determinant(matt) / detA;
			// check t
			result = (newt > 0);

			if (result)
			{
				// solve for alpha
				mat3 matalpha = {
					-be,
					-by,
					Vd
				};
				double alpha = determinant(matalpha) / detA;
				// check alpha
				result = result && (alpha >= 0) && (alpha <= 1);
				// solve for beta
				if (result)
				{
					mat3 matbeta = {
						-bx,
						-be,
						Vd
					};
					double beta = determinant(matbeta) / detA;
					// check beta
					result = result && (beta >= 0) && (beta <= 1) && ((beta + alpha) <= 1);
				}
			}
			
		}
		if ((result) && ((t == -1) || (t > newt)))
		{
			t = newt;
			inormalV = vec4(normalV, 0);
			if (!info.SecondaryMode)
			{
				info.IsMesh = true;
				info.IntersectedPrimitive = this;
				info.MeshTriangleIndex = i;
			}
		}
		flag_intersected = flag_intersected || result;
	}

	return flag_intersected;
}

void Mesh::setTrans(mat4 trans)
{
	this->trans = trans;
}

void Mesh::setTransNoScale(mat4 transNoScale)
{
	this->transNoScale = transNoScale;
}

bool Mesh::collisionDetection(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info)
{
	bool flag = false;

	// equation
	// At^2 + Bt + C = 0;
	vec3 m_pos = collisionMech_center;
	double m_radius = collisionMech_radius;
	
	vec4 Ve_t = inverse(trans) * vec4(theRay->getEye(), 1);
	vec4 Vd_t = inverse(trans) * vec4(theRay->getVd(), 0);
	vec3 Ve = vec3(Ve_t.x, Ve_t.y, Ve_t.z);
	vec3 Vd = vec3(Vd_t.x, Vd_t.y, Vd_t.z);

	double ex = Ve.x;
	double ey = Ve.y;
	double ez = Ve.z;
	double cx = m_pos.x;
	double cy = m_pos.y;
	double cz = m_pos.z;
	double dx = Vd.x;
	double dy = Vd.y;
	double dz = Vd.z;

	double eqA = square(dx) + square(dy) +square(dz);
	double eqB = (2 * (ex - cx) * dx) + (2 * (ey - cy) * dy) + (2 * (ez - cz) * dz);
	double eqC = square(ex - cx) + square(ey - cy) + square(ez - cz) - square(m_radius);

	// solve this equation
	double roots[2];
	size_t nroot = quadraticRoots(eqA, eqB, eqC, roots);

	flag = (nroot != 0);

	if (nroot == 1)
	{
		// touched
		t = roots[0];
		if (t < 0)
		{
			flag = false;
		}
	} else if (nroot == 2) {
		// pass through
		if (roots[0] > 0)
		{
			t = roots[0];
		} else if (roots[1] > 0) {
			t = roots[1];
		} else {
			flag = false;
		}

		if (flag)
		{
			if ((roots[0] > 0) && (roots[0] < t))
			{
				t = roots[0];
			}
			if ((roots[1] > 0) && (roots[1] < t))
			{
				t = roots[1];
			}
		}
	}

	if ((flag) && (!info.SecondaryMode))
	{
		vec3 point(Ve + t * Vd);
		inormalV = normalize(vec4((point - m_pos), 0));
		inormalV = normalize(transNoScale * inormalV);
		info.IsMesh = false;
		info.IntersectedPrimitive = this;
		info.MeshTriangleIndex = 0;
	}

	return flag;
}

double Mesh::maxdouble(double d1, double d2)
{
	if (d1 > d2)
	{
		return d1;
	} else {
		return d2;
	}
}

float Mesh::square(float s)
{
	return s * s;
}

PrimitiveType Mesh::getType()
{
	return PrimitiveType::PrimitiveMesh;
}

void Mesh::getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius)
{
	vec4 center4 = ptrans * vec4(collisionMech_center, 1);
	center = vec3(center4.x, center4.y, center4.z);
	vec4 p = vec4(collisionMech_center, 1) + vec4(0, 0, collisionMech_radius, 0);
	p = ptrans * p;
	boundRadius = distance(p, center4);
}

void Mesh::drawBound()
{
	bound = true;
}

void Mesh::drawMesh()
{
	bound = false;
}