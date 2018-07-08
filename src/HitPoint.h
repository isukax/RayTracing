#pragma once

#include "Constant.h"
#include "Vector3.h"

class Hitable;

struct HitPoint
{
public:
	std::shared_ptr<Hitable> object;
	Vector3 position;
	Vector3 normal;
	double distance;
	double u;
	double v;

	HitPoint()
		: position()
		, normal()
		, distance(kINF)
		, u()
		, v()
		, object(nullptr)
	{}
};

