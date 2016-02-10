#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "cs488-framework/MathUtils.hpp"
#include "UniversalShader.hpp"

#include "Light.hpp"
#include "Material.hpp"
#include "PhongMaterial.hpp"
#include "Tray.hpp"
#include "SceneNode.hpp"
#include "IntersectedPrimitiveInfo.hpp"

using namespace std;
using namespace glm;

vec3 UniversalShader::PhongShading(
		const vec3 &eye,
		const vec3 &view,
		const vec4 &normalV,
		const vec3 &ambient,
		const list<Light*> &lights,
		Material *theMaterial,
		const vec3 &pos,
		SceneNode * root,
		const IntersectedPrimitiveInfo &cinfo)
{
	vec3 pixel(0, 0, 0);
	// copy info
	IntersectedPrimitiveInfo info = cinfo;
	vec4 Vd = vec4(view, 0);
	vec3 enormalVector = vec3(normalV.x, normalV.y, normalV.z);
	PhongMaterial *myMaterial = (PhongMaterial*)theMaterial;
	// Phong shading
	for (Light *currentLight : lights)
	{
	    // construct a secondary ray
	    // from surface point to light source
	    Tray secondary(pos, currentLight->position - pos);
	    secondary.setIntersectedPrimitiveInfo(info);
	    vec3 diffuse(0, 0, 0);
	    vec3 specular(0.0);
	    // intersect secondary ray with meshes
	    bool blocked = (secondary.intersectSecondary(root)) && (secondary.getMint() > 0) && (secondary.getMint() < 1);
	    //blocked = false;
	    if (!blocked) {
	    	vec3 l = normalize(currentLight->position - pos);
	    	vec3 v = normalize(eye - pos);
	    	double light_distance = distance(currentLight->position, pos);
		    double n_dot_l = dot(l, enormalVector);
		    const double c1 = currentLight->falloff[0];
			const double c2 = currentLight->falloff[1];
			const double c3 = currentLight->falloff[2];
			// current light energy
		    vec3 lin = currentLight->colour / (c1 + c2 * light_distance + c3 * light_distance * light_distance);

		    if (n_dot_l < 0)
		    {
		    	n_dot_l = 0;
		    }

			diffuse = myMaterial->m_kd * n_dot_l * lin;

			vec3 r = -l + 2 * n_dot_l * enormalVector;
			float r_dot_v = dot(v, r);
		    if (r_dot_v < 0)
		    {
		        r_dot_v = 0;
		    }
		    specular = myMaterial->m_ks * pow(r_dot_v, myMaterial->m_shininess) * lin;
	    }
	    pixel += diffuse + specular;
	}

	pixel += ambient;

	return pixel;
}