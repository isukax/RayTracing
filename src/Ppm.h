#pragma once

#include<string>
#include "Constant.h"
#include "Vector3.h"
#include "Material.h"

class Ppm
{
public:
	static void Save(const std::string &filename, const Color *image, const int width, const int height)
	{
		CreateDirectory("./image");

		FILE *f = fopen(filename.c_str(), "wb");
		fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
		for (int i = 0; i < width * height; i++)
		{
			fprintf(f, "%d %d %d ", ToInt(image[i].x), ToInt(image[i].y), ToInt(image[i].z));
		}
		fclose(f);
	}

private:
	static inline double saturate(double x) 
	{
		if (x < 0.0)
			return 0.0;
		if (x > 1.0)
			return 1.0;
		return x;
	}

	static inline int ToInt(double x)
	{
		return int(LinearToGamma(x) * 255 + 0.5);
	}

	static inline double LinearToGamma(double x)
	{
		return pow(saturate(x), 1 / 2.2);
	}

	static inline void CreateDirectory(std::string name)
	{
		if (!_mkdir(name.c_str()))
		{
			std::cout << "create image folder : " << name << std::endl;
		}
	}
};




