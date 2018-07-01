#pragma once

#include "Vector3.h"
#include "HitPoint.h"
#include "Ray.h"
#include "Random.h"
#include "Constant.h"

//struct HitPoint;

using Color = Vector3;

class Material
{
public:
	enum class ReflectionType {
		REFLECTION_TYPE_DIFFUSE,	// ���S�g�U�ʁB������Lambertian�ʁB
		REFLECTION_TYPE_SPECULAR,	// ���z�I�ȋ��ʁB
		REFLECTION_TYPE_REFRACTION,	// ���z�I�ȃK���X�I�����B
	};

	Material(ReflectionType type, Color color, Color emission = Color())
		: reflType(type)
		, albedo(color)
		, emission(emission)
	{
	}
	
	virtual void GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, Vector3& scatterDir, Vector3& attenuation) = 0;

public:
	ReflectionType reflType;
	Color albedo;
	Color emission;
};

class LambertMaterial : public Material
{
public:
	LambertMaterial(Color albedo, Color emission = Color())
		: Material(ReflectionType::REFLECTION_TYPE_DIFFUSE, albedo, emission)
	{}

	virtual void GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, Vector3& scatterDir, Vector3& attenuation) override
	{
		// orienting_normal�̕�������Ƃ������K�������(w, u, v)�����B���̊��ɑ΂��锼�����Ŏ��̃��C���΂��B
		Vector3 w, u, v;
		const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // �����ʒu�̖@���i���̂���̃��C�̓��o���l���j
		w = normal;
		if (fabs(w.x) > kEPS)
		{
			u = Normalize(Cross(Vector3(0.0, 1.0, 0.0), w));
		}
		else
		{
			u = Normalize(Cross(Vector3(1.0, 0.0, 0.0), w));
		}
		v = Cross(w, u);

		double phi = 2.0 * kPI * rand.Next();
		double r2 = rand.Next(), theta = sqrt(r2);
		Vector3 dir = Normalize(
			u * cos(phi) * theta +
			v * sin(phi) * theta +
			w * sqrt(1.0 - r2)
		);

		scatterDir = dir;
		attenuation = albedo;
	}


};