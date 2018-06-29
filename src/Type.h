#pragma once

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


class Hitable
{
private:
	static uint32_t ObjectId;

public:
	Hitable()
		:objectId(ObjectId++)
	{}
	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) = 0;// { return false; }

public:
	uint32_t objectId;
};

uint32_t Hitable::ObjectId = 0;

