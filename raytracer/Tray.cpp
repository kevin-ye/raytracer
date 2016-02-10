#include "Tray.hpp"
#include "A4.hpp"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "cs488-framework/MathUtils.hpp"

#include "SceneNode.hpp"
#include "GeometryNode.hpp"
#include "Material.hpp"
#include "PhongMaterial.hpp"
#include "UniversalShader.hpp"
#include "IntersectedPrimitiveInfo.hpp"
#include "polyroots.hpp"

using namespace std;
using namespace glm;

Tray::Tray(const glm::vec3 & eye,
			const glm::vec3 & view,
			const glm::vec3 & up,
			const double fovy,
			const size_t imageH,
			const size_t imageW,
			const unsigned int px,
			const unsigned int py,
			const double dx,
			const double dy,
			const glm::vec3 & ambient,
			const list<Light *> &alllights)
: eye(eye), 
Vd(0),
intersected(NULL),
ambient(ambient),
lights(alllights),
mint(-1)
{
	// get surface point
	vec4 vSurfaceOrigin = glm::translate(view) * vec4(eye, 1);
	vec3 pSurfaceOrigin = vec3(vSurfaceOrigin[0], vSurfaceOrigin[1], vSurfaceOrigin[2]);

	double view_distance = distance(pSurfaceOrigin, eye);
	double half_fov = degreesToRadians(fovy / 2);

	double viewHeight = 2 * view_distance * tan(half_fov);
	double viewWidth = (double(imageW) / double(imageH)) * viewHeight;

	double pixelHeight = viewHeight / imageH;
	double pixelWidth = viewWidth / imageW;

	vec3 Vup = normalize(up);
	vec3 Vright = normalize(cross(view, up));

	vec3 surface_right = (px * pixelWidth - (viewWidth / 2) + (pixelWidth / 2) + dx * pixelWidth) * Vright;
	vec3 surface_up = ((viewHeight / 2) - py * pixelHeight - (pixelHeight / 2) + dy * pixelHeight) * Vup;

	Vd = surface_up + surface_right + view;
	//cout << to_string(surface_up) << "," << to_string(surface_right) << endl;
	//cout << to_string(Vright) << endl;
} 

Tray::Tray(const vec3 &source, const vec3 &direction) 
: eye(source), 
Vd(direction), 
intersected(NULL),
ambient(vec3(0, 0, 0)),
lights(list<Light*>()),
mint(-1)
{}

vec3 Tray::getVd()
{
	return Vd;
}

vec3 Tray::getEye()
{
	return eye;
}

bool Tray::RecursiveIntersect(SceneNode * node, mat4 trans, mat4 transNoScale)
{
	if (node == NULL)
	{
		return false;
	} else {
		bool flag = false;
		if (node->m_nodeType == NodeType::GeometryNode)
		{
			vec3 boundCenter;
			double boundRadius;
			node->getBound(trans, transNoScale, boundCenter, boundRadius);
			if ((boundRadius <= 0) ||intersectBoundingVolume(boundCenter, boundRadius))
			{
				// ray hits the bounding volume
				// try to intersect
				double newt = -1;
				vec4 resultNormalV;
				bool result = ((GeometryNode*)node)->intersect(this, resultNormalV, newt, info, trans, transNoScale);
				if ((result) && (newt > 0) && ((intersected == NULL) || (mint > newt)))
				{
					flag = true;
					// no t or smaller t
					this->mint = newt;
					this->NormalVector = resultNormalV;
					this->intersected = node;
				}
			}
		}
		for (SceneNode * child : node->children)
		{
			flag = RecursiveIntersect(child, trans * node->transNoScale, transNoScale * node->transNoScale) || flag;
		}
		return flag;
	}
}

vec3 Tray::intersect(SceneNode * root)
{
	mint = -1;
	this->intersected = NULL;
	info.IsMesh = false;
	info.MeshTriangleIndex = 0;
	info.IntersectedPrimitive = NULL;
	info.SecondaryMode = false;
	vec3 thePixel;
	if (RecursiveIntersect(root, mat4(1), mat4(1)))
	{
		// intersected
		vec3 point = eye + (mint - 0.00005) * Vd;
		GeometryNode *myNode = ((GeometryNode*)intersected);
		Material *myMaterial = myNode->m_material;
		// shading
		if (myMaterial->getMaterialType() == MaterialType::PhongMaterial)
		{
			// PhongMaterial, use Phong Shading
			// NormalVector = normalize(inverse(myNode->trans) * NormalVector);
			thePixel = UniversalShader::PhongShading(eye, Vd, NormalVector, ambient, lights, myMaterial, point, root, info);
		}
		
	} else {
		// no intersection
		thePixel = vec3(-1, -1, -1);
	}

	return thePixel;
}

bool Tray::intersectSecondary(SceneNode * root)
{
	mint = -1;
	this->intersected = NULL;
	info.SecondaryMode = true;
	return RecursiveIntersect(root, mat4(1), mat4(1));
}

vec3 Tray::getSource()
{
	return eye;
}

vec3 Tray::getDirection()
{
	return Vd;
}

double Tray::getMint()
{
	return mint;
}

void Tray::setIntersectedPrimitiveInfo(const IntersectedPrimitiveInfo &copyinfo)
{
	info.IsMesh = copyinfo.IsMesh;
	info.MeshTriangleIndex = copyinfo.MeshTriangleIndex;
	info.IntersectedPrimitive = copyinfo.IntersectedPrimitive;
	info.SecondaryMode = copyinfo.SecondaryMode;
}

IntersectedPrimitiveInfo Tray::getIntersectedPrimitiveInfo()
{
	return info;
}

bool Tray::intersectBoundingVolume(glm::vec3 center, double boundRadius) 
{
	// equation
	// At^2 + Bt + C = 0;
	vec3 m_pos = center;
	double m_radius = boundRadius;

	vec3 Ve = eye;

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

	return flag_intersected;
}

float Tray::square(float s)
{
	return s * s;
}