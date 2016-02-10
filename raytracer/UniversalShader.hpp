#pragma once

#include <cstdlib>
#include <cstdio>
#include <list>

#include <glm/glm.hpp>
#include "IntersectedPrimitiveInfo.hpp"

class UniversalShader
{

public:
	UniversalShader();
	~UniversalShader();

	static glm::vec3 PhongShading(
		const glm::vec3 &eye,
		const glm::vec3 &view,
		const glm::vec4 &normalV,
		const glm::vec3 &ambient,
		const std::list<class Light*> &lights,
		class Material *theMaterial,
		const glm::vec3 &pos,
		class SceneNode * root,
		const IntersectedPrimitiveInfo &cinfo);
	
};