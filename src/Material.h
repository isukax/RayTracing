#pragma once

#include <vector>
#include <functional>
#include "Constant.h"
#include "Vector3.h"
#include "Ray.h"
#include "HitPoint.h"
#include "Random.h"

using Color = Vector3;
class Hitable;

// normalは正規化済み前提
// dirは入射していく方向を指す
inline Vector3 Reflect(const Vector3& dir, const Vector3& normal)
{
	//return 2.0 * normal * Dot(-dir, normal) - dir;
	return dir - 2.0 * normal * Dot(dir, normal);
}

// スネルの法則
// iorは相対屈折率
inline Vector3 Refract(const Vector3& dir, const Vector3& normal, double ior)
{
	double VN = Dot(dir, normal);
	return -ior * (dir - VN * normal) - normal * sqrt(1.0 - (ior * ior) * (1.0 - VN * VN));

	//return Normalize(dir * ior -normal * (into ? 1.0 : -1.0) * (ddn * ior + sqrt(cos2t))));
}

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
	virtual Color GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, uint32_t depth, double russianRouletteProb, std::function<Color(Vector3 dir, Color weight)> func) = 0;

protected:
	// 単位半径をもつ球内の点のランダムサンプリング
	Vector3 RandomInUnitSphere(Random& rand)
	{
		Vector3 p;
		do {
			p = 2.0 * Vector3(rand.Next(), rand.Next(), rand.Next()) - Vector3(1.0, 1.0, 1.0);
		} while (p.LengthSquared() >= 1.0);
		return p;
	}

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

	virtual Color GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, uint32_t depth, double russianRouletteProb, std::function<Color(Vector3 dir, Color weight)> func) override
	{
		Vector3 dir;
		dir = ImportanceSampling(ray, hitpoint, rand);
		//dir = RandomInUnitSphere(rand);
		Color weight = albedo / russianRouletteProb;
		return func(dir, weight);
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
};

class MetalMaterial : public Material
{
public:
	MetalMaterial(Color albedo, Color emission = Color(), double roughness = 0)
		: Material(ReflectionType::REFLECTION_TYPE_SPECULAR, albedo, emission)
		, roughness(roughness)
	{}

	virtual Color GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, uint32_t depth, double russianRouletteProb, std::function<Color(Vector3 dir, Color weight)> func) override
	{
		const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）

		Vector3 reflectDir = Reflect(ray.direction, normal);
		Vector3 scatterDir = Normalize(reflectDir + roughness * RandomInUnitSphere(rand));
		Color weight = albedo / russianRouletteProb;
		return func(scatterDir, weight);
	}

private:
	double roughness;
};


class DielectricMaterial: public Material
{
public:
	DielectricMaterial(Color albedo, Color emission = Color(), double ior = 1.5)
		: Material(ReflectionType::REFLECTION_TYPE_SPECULAR, albedo, emission)
		, ior(ior)
	{}

	virtual Color GetRadiance(const Ray& ray, const HitPoint& hitpoint, Random& rand, uint32_t depth, double russianRouletteProb, std::function<Color(Vector3 dir, Color weight)> func) override
	{
		const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）
		const bool isIncidence = Dot(hitpoint.normal, normal) > 0.0; // レイがオブジェクトから出るのか、入るのか
	
		// 符号間違い絶対ある...

		const double iorAir = 1.0;
		const double iorIn = isIncidence ? iorAir : ior;
		const double iorOut = isIncidence ? ior : iorIn;
		const double relativeIor = iorIn / iorOut;
		const double VN = Dot(ray.direction, normal);
		const double cosTheta2 = 1.0 - relativeIor * relativeIor * (1.0 - VN * VN);
		const Vector3 reflectDir = Reflect(ray.direction, hitpoint.normal);

		// 全反射
		if (cosTheta2 < 0.0) {
			Color weight = albedo / russianRouletteProb;
			return func(reflectDir, weight);
		}

		Vector3 refractDir = Refract(-ray.direction, hitpoint.normal, relativeIor);
		refractDir = Normalize(ray.direction * ior - hitpoint.normal * (isIncidence ? 1.0 : -1.0) * (VN * ior + sqrt(cosTheta2)));

		const double f0 = Fresnel0(iorIn, iorOut);
		// VNにマイナスがつくのはray.directionが逆だから
		double cos = isIncidence ? -VN : Dot(refractDir, -1.0 * normal);
		// レイの運ぶ放射輝度は屈折率の異なる物体間を移動するとき、屈折率の比の二乗の分だけ変化する。
		const double fr = FresnelShlick(f0, cos);
		// 屈折方向の光が屈折してray.dirの方向に運ぶ割合
		const double tr = (1.0 - fr) * relativeIor * relativeIor;

		// 一定以上レイを追跡したら屈折と反射のどちらか一方を追跡する。（さもないと指数的にレイが増える）
		// ロシアンルーレットで決定する。
		const double prob = 0.25 + 0.5 * fr;
		if (depth > 2)
		{
			if (rand.Next() < prob)
			{
				Color weight = albedo / (prob * russianRouletteProb) * fr;
				return func(reflectDir, weight);
			}
			else
			{
				Color weight = albedo / ((1.0 - prob) * russianRouletteProb) * tr;
				return func(refractDir, weight);
			}
		}
		else
		{
			// 屈折と反射の両方を追跡
			Color weight = albedo / russianRouletteProb;
			return
				func(reflectDir, weight * fr) +
				func(refractDir, weight* tr);
		}
		return Color();	// ダミー
	}

private:
	inline double Fresnel0(double iorIn, double iorOut)
	{
		return pow(iorIn - iorOut, 2) / pow(iorIn + iorOut, 2);
	}

	inline double FresnelShlick(double f0, double cos)
	{
		return f0 + (1.0 - f0) * pow(1.0 - cos, 5);
	}

private:
	double ior;
};