#pragma once

#include "Vector3.h"
#include "Ray.h"
#include "Constant.h"
#include "HitPoint.h"
#include "Material.h"
#include "Util.h"
#include <ilmbase-2.2.1/include/ImathVec.h>
#include <ilmbase-2.2.1/include/ImathQuat.h>
#include <ilmbase-2.2.1/include/ImathVecAlgo.h>
#include <ilmbase-2.2.1/include/ImathMatrix.h>
#include<sce_vectormath\include\vectormath\scalar\cpp\vectormath_aos.h>

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
		//double theta = asin(hitpoint.normal.y);
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

class Triangle : public Hitable
{
public:
	Triangle() = delete;
	Triangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const MaterialPtr& material)
		: p0(p0)
		, p1(p1)
		, p2(p2)
		, uv0()
		, uv1()
		, uv2()
		, material(material)
	{
		edge1 = p1 - p0;
		edge2 = p2 - p0;
		faceNormal = Normalize(Cross(edge1, edge2));
	}

	Triangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, const Imath::V2d& uv0, const Imath::V2d& uv1, const Imath::V2d& uv2, const Vector3& normal0, const Vector3& normal1, const Vector3& normal2, const MaterialPtr& material)
		: p0(p0)
		, p1(p1)
		, p2(p2)
		, uv0(uv0)
		, uv1(uv1)
		, uv2(uv2)
		, normal0(normal0)
		, normal1(normal1)
		, normal2(normal2)
		, material(material)
	{
		edge1 = p1 - p0;
		edge2 = p2 - p0;
		faceNormal = Normalize(Cross(edge1, edge2));
	}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		const Vector3 op = ray.origin - p0;

#if 0
		// 平面方程式
		// 面上の座標計算
		//if (Dot(ray.direction, normal) >= 0.0001) return false;

		double t = Dot(p0 - ray.origin, normal) / Dot(ray.direction, normal);
		if (t < kEPS) return false;

		Vector3 pos = ray.origin + t * ray.direction;

		// 三角形内判定
		Vector3 pt = pos - p0;
		Vector3 c = Cross(edge1, pt);
		if (Dot(c, normal) < 0) return false;
		c = Cross(p2 - p1, pt);
		if (Dot(c, normal) < 0) return false;
		c = Cross(p0 - p2, pt);
		if (Dot(c, normal) < 0) return false;

		hitpoint.distance = t;
		hitpoint.position = ray.origin + ray.direction * hitpoint.distance;
		hitpoint.normal = Normalize(Cross(edge1, edge2));
		hitpoint.u = 0.5;
		hitpoint.v = 0.5;
#else
		double det = determinant(edge1, edge2, -ray.direction);
		if (det <= 0) return false;

		double u = determinant(op, edge2, -ray.direction) / det;
		if (u < 0 || u > 1) return false;

		double v = determinant(edge1, op, -ray.direction) / det;
		if (v < 0 ||  (u + v) > 1) return false;

		double t = determinant(edge1, edge2, op) / det;
		if (t < kEPS) return false;

		hitpoint.distance = t;
		hitpoint.position = ray.origin + ray.direction * hitpoint.distance;

		if (material->faceShading == Material::FaceShading::PHONG)
		{
			Vector3 normalTemp = (normal1 - normal0) * u + (normal2 - normal0) * v + normal0;
			hitpoint.normal = normalTemp;
		}
		else if (material->faceShading == Material::FaceShading::GOURAUD)
		{
			hitpoint.normal = Normalize(normal0 + normal1 + normal2);
		}
		else 
		{
			hitpoint.normal = faceNormal;
		}

		Imath::V2d uvTemp = (uv1 - uv0) * u + (uv2 - uv0) * v + uv0;
		hitpoint.u = uvTemp.x;
		hitpoint.v = 1-uvTemp.y;
#endif
		return true;
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		return material;
	}

private:
	Vector3 p0;
	Vector3 p1;
	Vector3 p2;
	Vector3 faceNormal;
	Vector3 normal0;
	Vector3 normal1;
	Vector3 normal2;
	Imath::V2d uv0;
	Imath::V2d uv1;
	Imath::V2d uv2;
	MaterialPtr material;

	Vector3 edge1;
	Vector3 edge2;
};


class Translate : public Hitable 
{
public:
	Translate(HitablePtr obj, Vector3 offset)
		: object(obj)
		, offset(offset)
	{}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		Ray rayOffset(ray.origin - offset, ray.direction);
		if (object->intersect(rayOffset, hitpoint))
		{
			hitpoint.position += offset;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		return object->GetMaterial();
	}

private:
	HitablePtr object;
	Vector3 offset;

};

// TODO Quaternion、Matrix 自作 ISPC
class Rotate : public Hitable
{
public:
	Rotate() = delete;
	Rotate(const HitablePtr obj, const Vector3& axis, const double radian)
		: object(obj)
		//, quat(Vectormath::Aos::Quat::rotation(radian, Vectormath::Aos::Vector3(axis.x, axis.y, axis.z)))
	{
		quat.setAxisAngle(Imath::V3d(axis.x, axis.y, axis.z), radian);
	}

	//inline const Imath::V3d rotate(const Imath::Quatd & quat, const Imath::V3d& vec)
	//{
	//	float tmpX, tmpY, tmpZ, tmpW;
	//	tmpX = (((vec.x) + (quat.v.y * vec.z)) - (quat.v.z * vec.y));
	//	tmpY = (((vec.y) + (quat.v.z * vec.x)) - (quat.v.x * vec.z));
	//	tmpZ = (((vec.z) + (quat.v.x * vec.y)) - (quat.v.y * vec.x));
	//	tmpW = (((quat.v.x * vec.x) + (quat.v.y * vec.y)) + (quat.v.z * vec.z));
	//	return Imath::V3d(
	//		((((tmpW * quat.v.x) + (tmpX * quat.r)) - (tmpY * quat.v.z)) + (tmpZ * quat.v.y)),
	//		((((tmpW * quat.v.y) + (tmpY * quat.r)) - (tmpZ * quat.v.x)) + (tmpX * quat.v.z)),
	//		((((tmpW * quat.v.z) + (tmpZ * quat.r)) - (tmpX * quat.v.y)) + (tmpY * quat.v.x))
	//		);
	//}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
#if 0
		using vec3 = Vectormath::Aos::Vector3;
		Vectormath::Aos::Quat revq = Vectormath::Aos::conj(quat);
		vec3 origin = Vectormath::Aos::rotate(revq, vec3(ray.origin.x, ray.origin.y, ray.origin.z));
		vec3 direction = Vectormath::Aos::rotate(revq, vec3(ray.direction.x, ray.direction.y, ray.direction.z));
		Vector3 o = Vector3(origin.getX(), origin.getY(), origin.getZ());
		Vector3 d = Vector3(direction.getX(), direction.getY(), direction.getZ());

		Ray rot_r(o, d);
		if (object->intersect(rot_r, hitpoint)) {
			vec3 pos = vec3(hitpoint.position.x, hitpoint.position.y, hitpoint.position.z);
			vec3 normal = vec3(hitpoint.normal.x, hitpoint.normal.y, hitpoint.normal.z);
			vec3 rpos = Vectormath::Aos::rotate(quat, pos);
			vec3 rnormal = Vectormath::Aos::rotate(quat, normal);
			hitpoint.position = Vector3(rpos.getX(), rpos.getY(), rpos.getZ());
			hitpoint.normal =Vector3(rnormal.getX(), rnormal.getY(), rnormal.getZ());
			return true;
		}
		else {
			return false;
		}
#else
		Imath::Quatd invQ = quat.inverse();
		Imath::V3d origin = invQ.rotateVector(Imath::V3d(ray.origin.x, ray.origin.y, ray.origin.z));
		Imath::V3d direction = invQ.rotateVector(Imath::V3d(ray.direction.x, ray.direction.y, ray.direction.z));
		//Imath::V3d origin = rotate(invQ, Imath::V3d(ray.origin.x, ray.origin.y, ray.origin.z));
		//Imath::V3d direction = rotate(invQ, Imath::V3d(ray.direction.x, ray.direction.y, ray.direction.z));
		
		Ray rotRay(Vector3(origin.x, origin.y, origin.z), Vector3(direction.x, direction.y, direction.z));
		if (object->intersect(rotRay, hitpoint))
		{
			Imath::V3d hPos = quat.rotateVector(Imath::V3d(hitpoint.position.x, hitpoint.position.y, hitpoint.position.z));
			Imath::V3d hNormal = quat.rotateVector(Imath::V3d(hitpoint.normal.x, hitpoint.normal.y, hitpoint.normal.z));
			//Imath::V3d hPos = rotate(quat, Imath::V3d(hitpoint.position.x, hitpoint.position.y, hitpoint.position.z));
			//Imath::V3d hNormal = rotate(quat, Imath::V3d(hitpoint.normal.x, hitpoint.normal.y, hitpoint.normal.z));
			hitpoint.position = Vector3(hPos.x, hPos.y, hPos.z);
			hitpoint.normal = Vector3(hNormal.x, hNormal.y, hNormal.z);
			return true;
		}
		else {
			return false;
		}
#endif
		return true;
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		return object->GetMaterial();
	}

	

private:
	HitablePtr object;
	//Vectormath::Aos::Quat quat;
	Imath::Quatd quat;
};


class Scale : public Hitable
{
public:
	Scale(HitablePtr obj, Vector3 scale)
		: object(obj)
		, scale(scale)
	{}

	virtual bool intersect(const Ray& ray, HitPoint& hitpoint) override
	{
		Ray rayOffset(ray.origin / scale, ray.direction);
		if (object->intersect(rayOffset, hitpoint))
		{
			hitpoint.position = hitpoint.position * scale;
			return true;
		}
		else
		{
			return false;
		}
	}

	virtual const MaterialPtr GetMaterial() const override
	{
		return object->GetMaterial();
	}

private:
	HitablePtr object;
	Vector3 scale;

};
