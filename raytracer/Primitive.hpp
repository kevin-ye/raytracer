#pragma once

#include <glm/glm.hpp>

#include "IntersectedPrimitiveInfo.hpp"

enum class PrimitiveType {
    PrimitiveSphere,
    PrimitiveCube,
    PrimitiveNonhierSphere,
    PrimitiveNonhierBox,
    PrimitiveMesh
};

class Primitive {
    public:
        virtual ~Primitive();

        // try to intersect the ray with current GeometryNode,
        // success on True and intersction position is stored in thePixel
        // failed on False
        virtual bool intersect(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info) = 0;
        virtual void setTrans(glm::mat4 trans) = 0;
        virtual void setTransNoScale(glm::mat4 transNoScale) = 0;

        virtual void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius) = 0;

        virtual PrimitiveType getType() = 0;
};

class Sphere : public Primitive {
        glm::mat4 trans;
        glm::mat4 transNoScale;
    public:
        virtual ~Sphere();

        bool intersect(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info);

        void setTrans(glm::mat4 trans);
        void setTransNoScale(glm::mat4 transNoScale);

        void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius);

        PrimitiveType getType();
};

class Cube : public Primitive {
        glm::mat4 trans;
        glm::mat4 transNoScale;
    public:
        virtual ~Cube();

        bool intersect(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info);

        void setTrans(glm::mat4 trans);
        void setTransNoScale(glm::mat4 transNoScale);

        void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius);

        PrimitiveType getType();
};

class NonhierSphere : public Primitive {
        glm::mat4 trans;
        glm::mat4 transNoScale;
        glm::vec3 m_pos;
        double m_radius;
    public:
        NonhierSphere(const glm::vec3& pos, double radius);
        virtual ~NonhierSphere();

        bool intersect(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info);

        void setTrans(glm::mat4 trans);
        void setTransNoScale(glm::mat4 transNoScale);

        void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius);

        PrimitiveType getType();
};

class NonhierBox : public Primitive {
        glm::mat4 trans;
        glm::mat4 transNoScale;
        glm::vec3 m_pos;
        double m_size;
    public:
        NonhierBox(const glm::vec3& pos, double size);
        
        virtual ~NonhierBox();

        bool intersect(class Tray *theRay, glm::vec4 &inormalV, double &t, IntersectedPrimitiveInfo &info);

        void setTrans(glm::mat4 trans);
        void setTransNoScale(glm::mat4 transNoScale);

        void getBound(glm::mat4 ptrans, glm::mat4 ptransNoScale, glm::vec3 &center, double &boundRadius);

        PrimitiveType getType();
};
