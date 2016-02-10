#pragma once

#include <glm/glm.hpp>

#include "SceneNode.hpp"
#include "Primitive.hpp"
#include "Material.hpp"
#include "IntersectedPrimitiveInfo.hpp"

class GeometryNode : public SceneNode {
public:
	GeometryNode( const std::string & name, Primitive *prim, 
		Material *mat = nullptr );

	void setMaterial( Material *material );

	// try to intersect the ray with current GeometryNode,
	// success on True and color is stored in thePixel
	// failed on False
	bool intersect(
		class Tray *theRay, 
		glm::vec4 &normalV, 
		double &mint, 
		IntersectedPrimitiveInfo &info, 
		glm::mat4 ptrans, 
		glm::mat4 ptransNoScale);

	void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius);

	Material *m_material;
	Primitive *m_primitive;
};
