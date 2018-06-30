#pragma once

#include <vector>
#include "Sphere.h"

std::vector<Hitable*> scene
{
	//new Sphere(Vector3(0, 0, 0),		.5,		Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(1, 0, 0))),
	//new Sphere(Vector3(0, -100.5, 0),	100,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(.2, .2, .2))),
	//new Sphere(Vector3(0, 2, 0),		.5,		Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(1, 1, 1), Color(36, 36, 36))),

	new Sphere(Vector3(1e5 + 1, 40.8, 81.6),	1e5,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(0.75, 0.25, 0.25))),	// 左
	new Sphere(Vector3(-1e5 + 99, 40.8, 81.6),	1e5,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(0.25, 0.25, 0.75))),	// 右
	new Sphere(Vector3(50, 40.8, 1e5),			1e5,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(0.75, 0.75, 0.75))),	// 奥
	new Sphere(Vector3(50, 40.8, -1e5 + 250),	1e5,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color())),					// 手前
	new Sphere(Vector3(50, 1e5, 81.6),			1e5,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(0.75, 0.75, 0.75))),	// 床
	new Sphere(Vector3(50, -1e5 + 81.6, 81.6),	1e5,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(0.75, 0.75, 0.75))),	// 天井
	new Sphere(Vector3(65, 20, 20),				20,		Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(0.25, 0.75, 0.25))), // 緑球
	//new Sphere(Vector3(27, 16.5, 47),			16.5,	Material(Material::ReflectionType::REFLECTION_TYPE_SPECULAR, Color(0.99, 0.99, 0.99))), // 鏡
	//new Sphere(Vector3(77, 16.5, 78),			16.5,	Material(Material::ReflectionType::REFLECTION_TYPE_REFRACTION,  Color(0.99, 0.99, 0.99))), //ガラス
	new Sphere(Vector3(50.0, 90.0, 81.6),		15.0,	Material(Material::ReflectionType::REFLECTION_TYPE_DIFFUSE, Color(), Color(36,36,36))), //照明
};

inline bool IntersectScene(Ray& ray, std::vector<Hitable*>& scene, HitPoint& hitpoint)
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