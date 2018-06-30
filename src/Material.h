#pragma once

#include "Vector3.h"

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
		, color(color)
		, emission(emission)
	{
	}

public:
	ReflectionType reflType;
	Color color;
	Color emission;
};