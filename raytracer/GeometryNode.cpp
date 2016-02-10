#include "GeometryNode.hpp"
#include "Tray.hpp"
#include "IntersectedPrimitiveInfo.hpp"

#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cmath>
#include <glm/glm.hpp>
#include <glm/ext.hpp>

#include "cs488-framework/MathUtils.hpp"

using namespace std;
using namespace glm;

//---------------------------------------------------------------------------------------
GeometryNode::GeometryNode(
	const std::string & name, Primitive *prim, Material *mat )
	: SceneNode( name )
	, m_material( mat )
	, m_primitive( prim )
{
	m_nodeType = NodeType::GeometryNode;
}

// 

void GeometryNode::setMaterial( Material *mat )
{
	// Obviously, there's a potential memory leak here.  A good solution
	// would be to use some kind of reference counting, as in the 
	// C++ shared_ptr.  But I'm going to punt on that problem here.
	// Why?  Two reasons:
	// (a) In practice we expect the scene to be constructed exactly
	//     once.  There's no reason to believe that materials will be
	//     repeatedly overwritten in a GeometryNode.
	// (b) A ray tracer is a program in which you compute once, and 
	//     throw away all your data.  A memory leak won't build up and
	//     crash the program.

	m_material = mat;
}

// try to intersect the ray with current GeometryNode,
	// success on True and color is stored in thePixel
	// failed on False
bool GeometryNode::intersect(
	Tray *theRay, 
	vec4 &normalV, 
	double &mint, 
	IntersectedPrimitiveInfo &info, 
	mat4 ptrans, 
	mat4 ptransNoScale)
{
	bool flag_intersected = false;
	m_primitive->setTrans(ptrans * trans);
	m_primitive->setTransNoScale(ptransNoScale * transNoScale);
	flag_intersected = m_primitive->intersect(theRay, normalV, mint, info);

	return flag_intersected;
}

void GeometryNode::getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius)
{
	m_primitive->getBound(ptrans * trans, ptransNoScale * transNoScale, center, boundRadius);
}