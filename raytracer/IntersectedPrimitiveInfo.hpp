#pragma once

#include <cstdlib>
#include <cstdio>

class IntersectedPrimitiveInfo
{
public:
	IntersectedPrimitiveInfo();
	~IntersectedPrimitiveInfo();
	
	class Primitive *IntersectedPrimitive;
	bool IsMesh;
	size_t MeshTriangleIndex;

	bool SecondaryMode;
};