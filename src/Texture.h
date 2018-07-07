#pragma once

#include <cstdint>
#include <memory>
#include "Type.h"
#include "Constant.h"
#include "Vector3.h"
#include "Ray.h"

class Texture;
using TexturePtr = std::shared_ptr<Texture>;

class Texture
{
public:
	virtual Vector3 value(double u, double v, const Vector3& p) const = 0;
};

class ColorTexture : public Texture 
{
public:
	ColorTexture(Color color)
		: color(color)
	{
	}
	
	virtual Vector3 value(double u, double v, const Vector3& p) const override
	{
		return color;
	}
	
private:
	Color color;
};

class CheckerTexture : public Texture
{
public:
	CheckerTexture(const TexturePtr t0, const TexturePtr t1, double freq)
		: oddTex(t0)
		, evenTex(t1)
		, freq(freq)
	{
	}

	virtual Vector3 value(double u, double v, const Vector3& p) const override
	{
		return (signbit(sin(freq * p.x) * sin(freq * p.y) * sin(freq * p.z)) ? oddTex : evenTex)->value(u, v, p);
	}

private:
	TexturePtr oddTex;
	TexturePtr evenTex;
	double freq;
};