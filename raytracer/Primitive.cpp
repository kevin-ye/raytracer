#include <cstdio>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "cs488-framework/MathUtils.hpp"

#include "Primitive.hpp"
#include "Tray.hpp"
#include "polyroots.hpp"
#include "IntersectedPrimitiveInfo.hpp"

using namespace std;
using namespace glm;

// forward declaration
float square(float s);

// extra

float square(float s)
{
	return s * s;
}

//--------------------------------------------------------------------------------------- 

// Primitive
Primitive::~Primitive() {}

//---------------------------------------------------------------------------------------

// Sphere

Sphere::~Sphere() {}

bool Sphere::intersect(Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info)
{
	// equation
	// At^2 + Bt + C = 0;
	if (info.SecondaryMode)
	{
		if ((!info.IsMesh) && (info.IntersectedPrimitive == this))
		{
			return false;
		}
	}
	vec3 m_pos(0, 0, 0);
	double m_radius = 1;
	
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

	bool flag_intersected = (nroot != 0);

	if (nroot == 1)
	{
		// touched
		t = roots[0];
	} else if (nroot == 2) {
		// pass through
		if (roots[0] > 0)
		{
			t = roots[0];
		} else if (roots[1] > 0) {
			t = roots[1];
		}

		if ((roots[1] > 0) && (roots[1] < t))
		{
			t = roots[1];
		}
		if ((roots[2] > 0) && (roots[2] < t))
		{
			t = roots[2];
		}
	}

	if (t < 0)
	{
		flag_intersected = false;
	}

	if ((flag_intersected) && !(info.SecondaryMode))
	{
		vec3 point(Ve + t * Vd);
		inormalV = normalize(vec4((point - m_pos), 0));
		inormalV = normalize(transNoScale * inormalV);
		info.IsMesh = false;
		info.IntersectedPrimitive = this;
		info.MeshTriangleIndex = 0;
	}

	return flag_intersected;
}

void Sphere::setTrans(glm::mat4 trans)
{
	this->trans = trans;
}

void Sphere::setTransNoScale(glm::mat4 transNoScale)
{
	this->transNoScale = transNoScale;
}

PrimitiveType Sphere::getType()
{
	return PrimitiveType::PrimitiveSphere;
}

void Sphere::getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius)
{
	vec4 center4 = ptrans * vec4(0, 0, 0, 1);
	center = vec3(center4.x, center4.y, center4.z);
	vec4 p1 = vec4(0, 0, 1, 1);
	vec4 p2 = vec4(0, 1, 0, 1);
	vec4 p3 = vec4(1, 0, 0, 1);
	p1 = ptrans * p1;
	p2 = ptrans * p2;
	p3 = ptrans * p3;
	double r1 = distance(p1, center4);
	double r2 = distance(p2, center4);
	double r3 = distance(p3, center4);
	boundRadius = r1;
	if (boundRadius < r2)
	{
		boundRadius = r2;
	}
	if (boundRadius < r3)
	{
		boundRadius = r3;
	}
}

//---------------------------------------------------------------------------------------

// Cube

Cube::~Cube() {}

bool Cube::intersect(Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info)
{
	mat4 utrans = trans;
	double m_size = 1;
	if (info.SecondaryMode)
	{
		if ((!info.IsMesh) && (info.IntersectedPrimitive == this))
		{
			return false;
		}
	}
	bool flag_intersected = false;

	vec3 Ve = theRay->getEye();
	vec3 Vd = theRay->getVd();

	// front face
	vec3 v1(0, m_size, m_size);
	vec3 v2(m_size, m_size, m_size);
	vec3 v3(0, 0, m_size);
	vec3 v4(m_size, 0, m_size);
	vec3 normalV(0, 0, 0);
	// transform v1, v2, v3, v4
	vec4 v1_4 = utrans * vec4(v1, 1);
	vec4 v2_4 = utrans * vec4(v2, 1);
	vec4 v3_4 = utrans * vec4(v3, 1);
	vec4 v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = -normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	bool result = false;
	double newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// back face
	v1 = vec3(0, m_size, 0);
	v2 = vec3(m_size, m_size, 0);
	v3 = vec3(0, 0, 0);
	v4 = vec3(m_size, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// top face
	v1 = vec3(0, m_size, m_size);
	v2 = vec3(m_size, m_size, m_size);
	v3 = vec3(0, m_size, 0);
	v4 = vec3(m_size, m_size, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// bottom face
	v1 = vec3(0, 0, m_size);
	v2 = vec3(m_size, 0, m_size);
	v3 = vec3(0, 0, 0);
	v4 = vec3(m_size, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = -normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// left face
	v1 = vec3(0, m_size, m_size);
	v2 = vec3(0, m_size, 0);
	v3 = vec3(0, 0, m_size);
	v4 = vec3(0, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// right face
	v1 = vec3(m_size, m_size, m_size);
	v2 = vec3(m_size, m_size, 0);
	v3 = vec3(m_size, 0, m_size);
	v4 = vec3(m_size, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = -normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	if ((flag_intersected) && (!info.SecondaryMode))
	{
		info.IsMesh = false;
		info.IntersectedPrimitive = this;
		info.MeshTriangleIndex = 0;
	}

	return flag_intersected;
}

void Cube::setTrans(glm::mat4 trans)
{
	this->trans = trans;
}

void Cube::setTransNoScale(glm::mat4 transNoScale)
{
	this->transNoScale = transNoScale;
}

PrimitiveType Cube::getType()
{
	return PrimitiveType::PrimitiveCube;
}

void Cube::getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius)
{
	vec4 center4 = ptrans * vec4(0.5, 0.5, 0.5, 1);
	center = vec3(center4.x, center4.y, center4.z);
	vec4 p = vec4(0, 0, 0, 1);
	p = ptrans * p;
	boundRadius = distance(p, center4);
}

//---------------------------------------------------------------------------------------

// NonhierSphere

NonhierSphere::NonhierSphere(const glm::vec3& pos, double radius): m_pos(pos), m_radius(radius) {}

NonhierSphere::~NonhierSphere() {}

bool NonhierSphere::intersect(Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info)
{
	// equation
	// At^2 + Bt + C = 0;
	if (info.SecondaryMode)
	{
		if ((!info.IsMesh) && (info.IntersectedPrimitive == this))
		{
			return false;
		}
	}

	mat4 utrans = trans * translate(m_pos);
	
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

	bool flag_intersected = (nroot != 0);

	if (nroot == 1)
	{
		// touched
		t = roots[0];
		if (t < 0)
		{
			flag_intersected = false;
		}
	} else if (nroot == 2) {
		// pass through
		if (roots[0] > 0)
		{
			t = roots[0];
		} else if (roots[1] > 0) {
			t = roots[1];
		} else {
			flag_intersected = false;
		}

		if (flag_intersected)
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

	if ((flag_intersected) && (!info.SecondaryMode))
	{
		vec3 point(Ve + t * Vd);
		inormalV = normalize(vec4((point - m_pos), 0));
		inormalV = normalize(transNoScale * inormalV);
		info.IsMesh = false;
		info.IntersectedPrimitive = this;
		info.MeshTriangleIndex = 0;
	}

	return flag_intersected;
}

void NonhierSphere::setTrans(glm::mat4 trans)
{
	this->trans = trans;
}

void NonhierSphere::setTransNoScale(glm::mat4 transNoScale)
{
	this->transNoScale = transNoScale;
}

PrimitiveType NonhierSphere::getType()
{
	return PrimitiveType::PrimitiveNonhierSphere;
}

void NonhierSphere::getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius)
{
	vec4 center4 = ptrans * vec4(m_pos, 1);
	center = vec3(center4.x, center4.y, center4.z);
	vec4 p1 = vec4(m_pos, 1) + vec4(0, 0, m_radius, 0);
	vec4 p2 = vec4(m_pos, 1) + vec4(0, m_radius, 0, 0);
	vec4 p3 = vec4(m_pos, 1) + vec4(m_radius, 0, 0, 0);
	p1 = ptrans * p1;
	p2 = ptrans * p2;
	p3 = ptrans * p3;
	double r1 = distance(p1, center4);
	double r2 = distance(p2, center4);
	double r3 = distance(p3, center4);
	boundRadius = r1;
	if (boundRadius < r2)
	{
		boundRadius = r2;
	}
	if (boundRadius < r3)
	{
		boundRadius = r3;
	}
}

//---------------------------------------------------------------------------------------

// NonhierBox

NonhierBox::NonhierBox(const glm::vec3& pos, double size): m_pos(pos), m_size(size) {}

NonhierBox::~NonhierBox() {}

bool NonhierBox::intersect(Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info)
{
	if (info.SecondaryMode)
	{
		if ((!info.IsMesh) && (info.IntersectedPrimitive == this))
		{
			return false;
		}
	}
	bool flag_intersected = false;

	vec3 Ve = theRay->getEye();
	vec3 Vd = theRay->getVd();

	mat4 utrans = trans * translate(m_pos);

	// front face
	vec3 v1(0, m_size, m_size);
	vec3 v2(m_size, m_size, m_size);
	vec3 v3(0, 0, m_size);
	vec3 v4(m_size, 0, m_size);
	vec3 normalV(0, 0, 0);
	// transform v1, v2, v3, v4
	vec4 v1_4 = utrans * vec4(v1, 1);
	vec4 v2_4 = utrans * vec4(v2, 1);
	vec4 v3_4 = utrans * vec4(v3, 1);
	vec4 v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = -normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	bool result = false;
	double newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// back face
	v1 = vec3(0, m_size, 0);
	v2 = vec3(m_size, m_size, 0);
	v3 = vec3(0, 0, 0);
	v4 = vec3(m_size, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// top face
	v1 = vec3(0, m_size, m_size);
	v2 = vec3(m_size, m_size, m_size);
	v3 = vec3(0, m_size, 0);
	v4 = vec3(m_size, m_size, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// bottom face
	v1 = vec3(0, 0, m_size);
	v2 = vec3(m_size, 0, m_size);
	v3 = vec3(0, 0, 0);
	v4 = vec3(m_size, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = -normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// left face
	v1 = vec3(0, m_size, m_size);
	v2 = vec3(0, m_size, 0);
	v3 = vec3(0, 0, m_size);
	v4 = vec3(0, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	// right face
	v1 = vec3(m_size, m_size, m_size);
	v2 = vec3(m_size, m_size, 0);
	v3 = vec3(m_size, 0, m_size);
	v4 = vec3(m_size, 0, 0);
	normalV = vec3(0, 0, 0);
	// transform v1, v2, v3, v4
	v1_4 = utrans * vec4(v1, 1);
	v2_4 = utrans * vec4(v2, 1);
	v3_4 = utrans * vec4(v3, 1);
	v4_4 = utrans * vec4(v4, 1);
	v1 = vec3(v1_4.x, v1_4.y, v1_4.z);
	v2 = vec3(v2_4.x, v2_4.y, v2_4.z);
	v3 = vec3(v3_4.x, v3_4.y, v3_4.z);
	v4 = vec3(v4_4.x, v4_4.y, v4_4.z);
	normalV = -normalize(cross(v1 - v2, v1 - v3));
	// check if intersect is possible
	result = false;
	newt = 0;
	if (dot(Vd, normalV) != 0)
	{
		mat3 matA = {
			v3 - v4,
			v3 - v1,
			Vd
		};
		double detA = determinant(matA);
		// solve for t
		mat3 matt = {
			v3 - v4,
			v3 - v1,
			v3 - Ve
		};
		newt = determinant(matt) / detA;
		// check t
		result = (newt > 0);

		if (result)
		{
			// solve for alpha
			mat3 matalpha = {
				v3 - Ve,
				v3 - v1,
				Vd
			};
			double alpha = determinant(matalpha) / detA;
			// check alpha
			result = result && (alpha >= 0) && (alpha <= 1);
			// solve for beta
			if (result)
			{
				mat3 matbeta = {
					v3 - v4,
					v3 - Ve,
					Vd
				};
				double beta = determinant(matbeta) / detA;
				// check alpha
				result = result && (beta >= 0) && (beta <= 1);
			}
			// check beta
		}
		
	}
	if ((result) && ((t == -1) || (t > newt)))
	{
		t = newt;
		inormalV = vec4(normalV, 0);
	}
	flag_intersected = flag_intersected || result;

	if ((flag_intersected) && (!info.SecondaryMode))
	{
		info.IsMesh = false;
		info.IntersectedPrimitive = this;
		info.MeshTriangleIndex = 0;
	}

	return flag_intersected;
}

void NonhierBox::setTrans(glm::mat4 trans)
{
	this->trans = trans;
}

void NonhierBox::setTransNoScale(glm::mat4 transNoScale)
{
	this->transNoScale = transNoScale;
}

PrimitiveType NonhierBox::getType()
{
	return PrimitiveType::PrimitiveNonhierBox;
}

void NonhierBox::getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius)
{
	vec4 center4 = ptrans * m_size * vec4(0.5, 0.5, 0.5, 1);
	center = vec3(center4.x, center4.y, center4.z);
	vec4 p = vec4(m_pos, 1) + vec4(0, 0, 0, 0);
	p = ptrans * p;
	boundRadius = distance(p, center4);
}

//---------------------------------------------------------------------------------------