#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "Constant.h"
#include "HitPoint.h"
#include "Hitable.h"

class Rect : public Hitable
{
public:
	enum AxisType
	{
		XY,
		YZ,
		XZ,
	};

public:
	Rect() = delete;
	Rect(double w0, double w1, double h0, double h1, double k, AxisType type, MaterialPtr material)
		: Hitable(material)
		, w0(w0)
		, w1(w1)
		, h0(h0)
		, h1(h1)
		, k(k)
		, axisType(type)
	{
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		double t;
		double w, h;
		Vector3 axis;

		switch (axisType)
		{
		case XY:
			t = (k - ray.origin.z) / ray.direction.z;
			w = ray.origin.x + t * ray.direction.x;
			h = ray.origin.y + t * ray.direction.y;
			axis = Vector3(0, 0, 1);
			break;
		case YZ:
			t = (k - ray.origin.x) / ray.direction.x;
			w = ray.origin.z + t * ray.direction.z;
			h = ray.origin.y + t * ray.direction.y;
			axis = Vector3(1, 0, 0);
			break;
		case XZ:
			t = (k - ray.origin.y) / ray.direction.y;
			w = ray.origin.x + t * ray.direction.x;
			h = ray.origin.z + t * ray.direction.z;
			axis = Vector3(0, 1, 0);
			break;
		}

		if (t < kEPS) return false;

		if (t < 0 || t > DBL_MAX) {
			return false;
		}

		if (w < w0 || w > w1 || h < h0 || h > h1)
		{
			return false;
		}

		hitpoint.distance = t;
		hitpoint.position = ray.origin + ray.direction * hitpoint.distance;
		hitpoint.normal = axis;
		hitpoint.u = (w - w0) / (w1 - w0);
		hitpoint.v = (h - h0) / (h1 - h0);
		return true;
	}

public:
	double w0;
	double w1;
	double h0;
	double h1;
	double k;
	AxisType axisType;
};

