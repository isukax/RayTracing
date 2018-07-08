#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "Constant.h"
#include "HitPoint.h"
#include "Material.h"

class Hitalble;
using HitablePtr = std::shared_ptr<Hitable>;

class Hitable
{
private:
	static uint32_t ObjectId;

public:
	Hitable()
		: objectId(ObjectId++)
		//, material(material)
	{
	}

	~Hitable()
	{
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) = 0;
	virtual const MaterialPtr GetMaterial() const = 0;

public:
	uint32_t objectId;
};

uint32_t Hitable::ObjectId = 0;

class ShapeList : public Hitable
{
public:
	ShapeList()
		: Hitable()
		, list()
	{}

	void Add(HitablePtr object) { list.push_back(object); }

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		HitablePtr hitObject = nullptr;
		for (auto& obj : list)
		{
			HitPoint h;
			if (!obj->intersect(ray, h))
			{
				continue;
			}

			if (h.distance < hitpoint.distance)
			{
				hitpoint = h;
				if (h.object == nullptr)
				{
					hitpoint.object = obj;
					//hitpoint.object = h.object;
				}
				hitObject = obj;
			}
		}

		if (hitObject == nullptr)
		{
			return false;
		}
		return true;
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		MaterialPtr stb;
		return stb;
	}

private:
	std::vector<HitablePtr> list;
};

class Sphere : public Hitable
{
public:
	Sphere() = delete;
	Sphere(const Vector3 pos, const double radius, MaterialPtr material)
		: Hitable()
		, position(pos)
		, radius(radius)
		, material(material)
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
		double phi = atan2(hitpoint.normal.x, hitpoint.normal.z);
		double theta = asin(hitpoint.normal.y);
		hitpoint.u = 1.0 - (phi + kPI) / kPI2;
		hitpoint.v = (theta + kPI / 2.0) / kPI;
		
		return true;

		//Vector3 amc = (ray.origin - position);
		//double a = Dot(ray.direction, ray.direction);
		//double b = 2 * Dot(ray.direction, amc);
		//double c = Dot(amc, amc) - (radius * radius);

		//// 判別式
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

	virtual const MaterialPtr GetMaterial() const override
	{
		return material;
	}

public:
	Vector3 position;
	double radius;
	MaterialPtr material;
};

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
		: Hitable()
		, w0(w0)
		, w1(w1)
		, h0(h0)
		, h1(h1)
		, k(k)
		, axisType(type)
		, material(material)
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

	virtual const MaterialPtr GetMaterial() const override
	{
		return material;
	}

public:
	double w0;
	double w1;
	double h0;
	double h1;
	double k;
	AxisType axisType;
	MaterialPtr material;
};

class Box : public Hitable
{
public:
	Box() = delete;
	Box(const Vector3& topLeft, const Vector3& bottomRight, const MaterialPtr& material)
		: list()
		, material(material)
	{
		list.Add(std::make_shared<Rect>(topLeft.x, bottomRight.x, topLeft.y, bottomRight.y, topLeft.z, Rect::AxisType::XY, material));
		list.Add(std::make_shared<Rect>(topLeft.x, bottomRight.x, topLeft.y, bottomRight.y, bottomRight.z, Rect::AxisType::XY, material));
		list.Add(std::make_shared<Rect>(topLeft.z, bottomRight.z, topLeft.y, bottomRight.y, topLeft.x, Rect::AxisType::YZ, material));
		list.Add(std::make_shared<Rect>(topLeft.z, bottomRight.z, topLeft.y, bottomRight.y, bottomRight.x, Rect::AxisType::YZ, material));
		list.Add(std::make_shared<Rect>(topLeft.x, bottomRight.x, topLeft.z, bottomRight.z, topLeft.y, Rect::AxisType::XZ, material));
		list.Add(std::make_shared<Rect>(topLeft.x, bottomRight.x, topLeft.z, bottomRight.z, bottomRight.y, Rect::AxisType::XZ, material));
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		return list.intersect(ray, hitpoint);
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		return material;
	}


private:
	ShapeList list;
	MaterialPtr material;
};