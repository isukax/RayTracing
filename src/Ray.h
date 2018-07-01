#pragma once

#include "Vector3.h"

class Ray
{
public:
	Ray(const Vector3& orig, const Vector3& dir)
		: origin(orig)
		, direction(dir)
	{}

	Vector3 origin;
	Vector3 direction;
};
