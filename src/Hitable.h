#pragma once

#include <cstdint>
#include "Constant.h"
#include "HitPoint.h"
#include "Vector3.h"
#include "Ray.h"
#include "Material.h"

class Hitable
{
private:
	static uint32_t ObjectId;
	
public:
	Hitable(Material* material)
		: objectId(ObjectId++)
		, material(material)
	{
	}
	~Hitable()
	{
		delete material;
	}
	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) = 0;

public:
	uint32_t objectId;
	Material* material;
};

uint32_t Hitable::ObjectId = 0;


