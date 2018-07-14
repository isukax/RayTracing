#pragma once
#define STB_IMAGE_IMPLEMENTATION

#include <cstdint>
#include <memory>
#include "Type.h"
#include "Constant.h"
#include "Vector3.h"
#include "Ray.h"
#include <stb/stb_image.h>

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

class ImageTexture : public Texture
{
public:
	ImageTexture(std::string fileName)
	{
		int n;
		std::string path = RESOURCE_DIR + fileName;
		texels = stbi_load((path).c_str(), &width, &height, &n, 0);
		if (texels == nullptr)
		{
			std::cerr << "NOT FOUND TEXTURE : " << path.c_str() << std::endl;
			texels = new unsigned char[3] { 255, 255, 255 };
			width = 1;
			height = 1;
		}
	}

	~ImageTexture()
	{
		stbi_image_free(texels);
	}

	virtual Vector3 value(double u, double v, const Vector3& p) const override
	{
		int s = int(u * width);
		int t = int((1 - v) * height - 0.001);
		return sample(s, t);
	}

	Color sample(int s, int t) const
	{
		s = (s < 0) ? 0 : (s >= width) ? width - 1 : s;
		t = (t < 0) ? 0 : (t >= height) ? height - 1 : t;

		return Color(
			int(texels[3 * s + 3 * width * t]) / 255.0,
			int(texels[3 * s + 3 * width * t + 1]) / 255.0,
			int(texels[3 * s + 3 * width * t + 2]) / 255.0
		);
	}

private:
	int width;
	int height;
	unsigned char* texels;
};

using ImageTexturePtr = std::shared_ptr<ImageTexture>;