#include <iostream>
#include <fstream>
#include "Vector3.h"
#include "Ray.h"
#include "Sphere.h"
#include "Scene.h"

// namespace
// const

inline double DegreeToRadian(double degree)
{
	return degree * PI / 180.0;
}

Color color(Ray& ray, std::vector<Hitable*>& objectList)
{
	HitPoint hitpoint;
	bool isIntersectAnything = false;
	Color ret;

	if (!IntersectScene(ray, objectList, hitpoint))
	{
		float t = 0.5 * (-ray.direction.y + 1.0);
		return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
	}

	return -hitpoint.normal * 0.5 + Color(0.5, 0.5, 0.5);
}

inline double saturate(double x) {
	if (x < 0.0)
		return 0.0;
	if (x > 1.0)
		return 1.0;
	return x;
}

inline int ToInt(double x)
{
	return int(pow(saturate(x), 1 / 2.2) * 255 + 0.5);
}	

int main(int argc, char** argv)
{
	std::cout << "start" << std::endl;

	const uint32_t width = 320;//1024;
	const uint32_t height = 240;// 768;
	const double aspectRatio = float(width) / float(height);

	const Vector3 camPos(0, 0, -10);
	const Vector3 targetPos(0, 0, 0);
	const Vector3 camUp(0, 1, 0);
	//const double fov = DegreeToRadian(60.0);
	//const Vector3 camDir(0, 0, 1);

	const auto camDir	= Normalize(targetPos - camPos);
	const auto rightDir	= Normalize(Cross(camUp, camDir));
	const auto upDir	= Cross(camDir, rightDir);

	const double focalLength = 40.0;	// mm
	const double focalPlane = 30.0;		// mm

	// イメージセンサー
	const double screenWidth = focalPlane * aspectRatio;
	const double screenHeight = focalPlane;

	const Vector3 screenX = rightDir * screenWidth;
	const Vector3 screenY = upDir * screenHeight;
	const Vector3 screenCenter = camPos + camDir * focalLength;

	// OpenMP
	// Monte Carlo
	// Biased Unbiased
	// Super Sampling
	// Importance Sampling
	// Russian roulette
	// Bidirectional Path tracing
	// BVH KDTREE

	std::ofstream ofs("result.ppm");

	ofs << "P3\n" << width << " " << height << "\n255\n";

	for (auto y = 0; y < height; ++y)
	{
		for (auto x = 0; x < width; ++x)
		{
			// -0.5は中心基準になっているため
			Vector3 screenPos = screenCenter +
				(screenX * ((double)x / width - 0.5)) +
				(screenY * ((double)y / height - 0.5));
			Vector3 rayDir = Normalize(screenPos - camPos);

			Ray ray = Ray(camPos, rayDir);

			Color col = color(ray, scene);

			ofs << ToInt(col.x) << " " << ToInt(col.y) << " " << ToInt(col.z) << "\n";
		}
	}

	ofs.close();

	std::cout << "end" << std::endl;

    return 0;
}