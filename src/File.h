#pragma once

#include<string>
#include "Constant.h"
#include "Vector3.h"
#include "Material.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include<stb/stb_image_write.h>

class File
{
public:
	enum class Extension
	{
		PNG,
		PPM
	};

	static void Save(const std::string &filename, const Color *image, const int width, const int height, const Extension extension = Extension::PNG)
	{
		CreateDirectory("./image");

		switch (extension)
		{
		case Extension::PNG:
			SavePNG(filename, image, width, height);
			break;
		case Extension::PPM:
			SavePPM(filename, image, width, height);
			break;
		}
	}

private:
	static inline void SavePNG(const std::string &filename, const Color *image, const int width, const int height)
	{
		unsigned char* ptr = new unsigned char[width * height * 3];
		memset(ptr, 0, width * height * 3);
		int count = 0;
		for (int i = 0; i < width * height * 3; i += 3)
		{
			ptr[i] = static_cast<unsigned char>(ToInt(image[count].x));
			ptr[i + 1] = static_cast<unsigned char>(ToInt(image[count].y));
			ptr[i + 2] = static_cast<unsigned char>(ToInt(image[count].z));
			++count;
		}
		std::string path = filename + ".png";
		stbi_write_png(path.c_str(), width, height, 3, (void*)ptr, 3 * width);
		delete[] ptr;
	}

	static inline void SavePPM(const std::string &filename, const Color *image, const int width, const int height)
	{
		std::string path = filename + ".ppm";
		FILE *f = fopen(path.c_str(), "wb");
		fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
		for (int i = 0; i < width * height; i++)
		{
			fprintf(f, "%d %d %d ", ToInt(image[i].x), ToInt(image[i].y), ToInt(image[i].z));
		}
		fclose(f);
	}
	
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




