#pragma once

#include "Constant.h"
#include "Vector3.h"

struct HitPoint
{
public:
	uint32_t objectId;
	Vector3 position;
	Vector3 normal;
	double distance;

	HitPoint()
		: objectId(-1)
		, position()
		, normal()
		, distance(kINF)
	{}
};

