#pragma once

#include "Vector3.h"

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
		, color(color)
		, emission(emission)
	{
	}

public:
	ReflectionType reflType;
	Color color;
	Color emission;
};