#include "IntersectedPrimitiveInfo.hpp"

#include <cstdlib>
#include <cstdio>

IntersectedPrimitiveInfo::IntersectedPrimitiveInfo()
: IntersectedPrimitive(NULL),
IsMesh(false),
MeshTriangleIndex(0),
SecondaryMode(false)
{}

IntersectedPrimitiveInfo::~IntersectedPrimitiveInfo() {}