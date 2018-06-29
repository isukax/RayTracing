#pragma once

#include <vector>
#include "Sphere.h"

std::vector<Hitable*> scene
{
	new Sphere(Vector3(0, 0, 0), .5, Vector3(1, 0, 0))
};

bool IntersectScene(Ray& ray, std::vector<Hitable*>& scene, HitPoint& hitpoint)
{
	HitPoint h;
	
	for (auto& obj : scene)
	{
		if (!obj->intersect(ray, h))
		{
			continue;
		}

		if (h.distance < hitpoint.distance)
		{
			hitpoint = h;
			hitpoint.objectId = obj->objectId;
		}
	}

	if (hitpoint.objectId == -1)
	{
		return false;
	}
	return true;

}