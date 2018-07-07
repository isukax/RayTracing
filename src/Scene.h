#pragma once

#include <vector>
#include <memory>
#include "Hitable.h"
#include "Sphere.h"
#include "Texture.h"

std::vector<HitablePtr> scene
{
	//std::make_shared<Sphere>(Vector3(0, 0, 0),		.5,		Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(1, 0, 0))),
	//new Sphere(Vector3(0, -100.5, 0),	100,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(.2, .2, .2))),
	//new Sphere(Vector3(0, 2, 0),		.5,		Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(1, 1, 1), Color(36, 36, 36))),

	////new Sphere(Vector3(27, 16.5, 47),			16.5,	Material(Material::ReflectionType::REFLECTION_TYPE_SPECULAR, Color(0.99, 0.99, 0.99))), // 鏡
	////new Sphere(Vector3(77, 16.5, 78),			16.5,	Material(Material::ReflectionType::REFLECTION_TYPE_REFRACTION,  Color(0.99, 0.99, 0.99))), //ガラス

	std::make_shared<Sphere>(Vector3(1e5 + 1, 40.8, 81.6),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.25, 0.25)))),	// 左
	std::make_shared<Sphere>(Vector3(-1e5 + 99, 40.8, 81.6),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.25, 0.75)))),	// 右
	std::make_shared<Sphere>(Vector3(50, 40.8, 1e5),			1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))),	// 奥
	std::make_shared<Sphere>(Vector3(50, 40.8, -1e5 + 250),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()))),					// 手前
	//std::make_shared<Sphere>(Vector3(50, 1e5, 81.6),			1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))),	// 床
	std::make_shared<Sphere>(Vector3(50, 1e5, 81.6),			1e5,	std::make_shared<LambertMaterial>(std::make_shared<CheckerTexture>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)), std::make_shared<ColorTexture>(Color(0, 0, 0)), .5))),	// 床

	std::make_shared<Sphere>(Vector3(50, -1e5 + 81.6, 81.6),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))),	// 天井
	std::make_shared<Sphere>(Vector3(65, 20, 20),				20,		std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.75, 0.25)))), // 緑球
	//std::make_shared<Sphere>(Vector3(65, 20, 20),				20,		std::make_shared<LambertMaterial>(Color(0.25, 0.75, 0.25))), // 緑球
	std::make_shared<Sphere>(Vector3(27, 16.5, 47),			16.5,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
	std::make_shared<Sphere>(Vector3(77, 16.5, 78),			16.5,	std::make_shared<DielectricMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)))), //ガラス
	std::make_shared<Sphere>(Vector3(50.0, 90.0, 81.6),		15.0,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()), Color(36,36,36))), //照明
};

inline bool IntersectScene(Ray& ray, std::vector<HitablePtr>& scene, HitPoint& hitpoint)
{
	for (auto& obj : scene)
	{
		HitPoint h;
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