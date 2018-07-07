#pragma once

#include <cstdint>
#include "Constant.h"
#include "HitPoint.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"

class Hitalble;
using HitablePtr = std::shared_ptr<Hitable>;

class Hitable
{
private:
	static uint32_t ObjectId;
	
public:
	Hitable(MaterialPtr material)
		: objectId(ObjectId++)
		, material(material)
	{
	}

	~Hitable()
	{
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) = 0;

public:
	uint32_t objectId;
	MaterialPtr material;
};

uint32_t Hitable::ObjectId = 0;


