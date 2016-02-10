#pragma once

#include <vector>
#include <iosfwd>
#include <string>

#include <glm/glm.hpp>

#include "Primitive.hpp"
#include "IntersectedPrimitiveInfo.hpp"

struct Triangle
{
	size_t v1;
	size_t v2;
	size_t v3;

	Triangle( size_t pv1, size_t pv2, size_t pv3 )
		: v1( pv1 )
		, v2( pv2 )
		, v3( pv3 )
	{}
};

// A polygonal mesh.
class Mesh : public Primitive {
			glm::mat4 trans;
    	    glm::mat4 transNoScale;
			std::vector<glm::vec3> m_vertices;
			std::vector<Triangle> m_faces;

			glm::vec3 collisionMech_center;
			double collisionMech_radius;

		    friend std::ostream& operator<<(std::ostream& out, const Mesh& mesh);

		    double maxdouble(double d1, double d2);
		    float square(float s);

		    bool collisionDetection(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info);

		    bool bound;
	public:
		  Mesh( const std::string& fname );
		  bool intersect(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info);

		  void setTrans(glm::mat4 trans);
          void setTransNoScale(glm::mat4 transNoScale);

          void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius);

          PrimitiveType getType();

          void drawBound();
          void drawMesh();
};
