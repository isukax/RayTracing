#pragma once

#include "Constant.h"
#include "Vector3.h"
#include "Ray.h"
#include "HitPoint.h"
#include "Random.h"

using Color = Vector3;

class Material
{
public:
	enum class ReflectionType {
		REFLECTION_TYPE_DIFFUSE,	// 完全拡散面。いわゆるLambertian面。
		REFLECTION_TYPE_SPECULAR,	// 理想的な鏡面。
		REFLECTION_TYPE_REFRACTION,	// 理想的なガラス的物質。
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
		Vector3 dir;
		dir = ImportanceSampling(ray, hitpoint, rand);
		//dir = RandomInUnitSphere(rand);
		scatterDir = dir;
		attenuation = albedo;
	}
private:
	// 重点サンプリング(半球)
	// orienting_normalの方向を基準とした正規直交基底(w, u, v)を作る。この基底に対する半球内で次のレイを飛ばす。
	Vector3 ImportanceSampling(const Ray& ray, const HitPoint& hitpoint, Random& rand)
	{
		Vector3 w, u, v;
		const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）
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
		return dir;
	}

	// 単位半径をもつ球内の点のランダムサンプリング
	Vector3 RandomInUnitSphere(Random& rand)
	{
		Vector3 p;
		do {
			p = 2.0 * Vector3(rand.Next(), rand.Next(), rand.Next()) - Vector3(1.0, 1.0, 1.0);
		} while (p.LengthSquared() >= 1.0);
		return p;
	}
};

class MetalMaterial : public Material
{
public:
	MetalMaterial(Color albedo, Color emission = Color())
		: Material(ReflectionType::REFLECTION_TYPE_SPECULAR, albedo, emission)
	{}

	virtual void GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, Vector3& scatterDir, Vector3& attenuation) override
	{
	}
};