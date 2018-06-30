#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "Material.h"
#include "Constant.h"
#include "Type.h"

class Sphere : public Hitable
{
public:
	Sphere() = delete;
	Sphere(const Vector3 pos, const double radius, const Material material)
		: Hitable(material)
		, position(pos)
		, radius(radius)
	{
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		const Vector3 p_o = position - ray.origin;
		const double b = Dot(p_o, ray.direction);
		const double D4 = b * b - Dot(p_o, p_o) + radius * radius;

		if (D4 < 0.0)
			return false;

		const double sqrt_D4 = sqrt(D4);
		const double t1 = b - sqrt_D4, t2 = b + sqrt_D4;

		if (t1 < kEPS && t2 < kEPS)
			return false;

		if (t1 > kEPS) {
			hitpoint.distance = t1;
		}
		else {
			hitpoint.distance = t2;
		}

		hitpoint.position = ray.origin + ray.direction * hitpoint.distance;
		hitpoint.normal = Normalize(hitpoint.position - position);
		return true;

		//Vector3 amc = (ray.origin - position);
		//double a = Dot(ray.direction, ray.direction);
		//double b = 2 * Dot(ray.direction, amc);
		//double c = Dot(amc, amc) - (radius * radius);

		//// ”»•ÊŽ®
		//double discriminant = b * b - 4 * a * c;
		//if (discriminant < 0)
		//{
		//	return false;
		//}

		//double sqrtD = sqrt(discriminant);
		//double t1 = (-b - sqrtD) / (2 * a);
		//double t2 = (-b + sqrtD) / (2 * a);
		//if (t1 < kEPS && t2 < kEPS)
		//	return false;
		//if (t1 > kEPS)
		//{
		//	hitpoint.distance = t1;
		//}
		//else
		//{
		//	hitpoint.distance = t2;
		//}

		//hitpoint.position = ray.origin + ray.direction * hitpoint.distance;
		//hitpoint.normal = Normalize(hitpoint.position - position);

		//return true;
	}

public:
	Vector3 position;
	double radius;

};

