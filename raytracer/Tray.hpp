#pragma once

#include <cstdlib>
#include <cstdio>
#include <list>

#include <glm/glm.hpp>

#include "IntersectedPrimitiveInfo.hpp"

class Tray {

	glm::vec3 eye;
	glm::vec3 Vd;

	double mint; 
	glm::vec4 NormalVector;
	IntersectedPrimitiveInfo info;

	const glm::vec3 &ambient;
	const std::list<class Light*> &lights;

	bool RecursiveIntersect(class SceneNode *node, glm::mat4 trans, glm::mat4 transNoScale);
	float square(float s);

	public:
		Tray(
			const glm::vec3 & eye,
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
			const std::list<class Light *> &alllights);
		Tray(
			const glm::vec3 & source,
			const glm::vec3 & direction);

		glm::vec3 getVd();
		glm::vec3 getEye();
		glm::vec3 getSource();
		glm::vec3 getDirection();
		double getMint();
		glm::vec3 intersect(class SceneNode * root);
		bool intersectSecondary(class SceneNode * root);
		class SceneNode *intersected;

		void setIntersectedPrimitiveInfo(const IntersectedPrimitiveInfo &copyinfo);
		IntersectedPrimitiveInfo getIntersectedPrimitiveInfo();

		bool intersectBoundingVolume(glm::vec3 center, double boundRadius);
};