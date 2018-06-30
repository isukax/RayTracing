#include <iostream>
#include <fstream>
#include "Vector3.h"
#include "Ray.h"
#include "Sphere.h"
#include "Scene.h"
#include "Camera.h"
#include "Random.h"
#include <algorithm>

// namespace
// const

inline double DegreeToRadian(double degree)
{
	return degree * PI / 180.0;
}

Color color(Ray& ray, std::vector<Hitable*>& objectList, Random& rand, uint32_t depth)
{
	HitPoint hitpoint;
	bool isIntersectAnything = false;
	Color ret;

	if (!IntersectScene(ray, objectList, hitpoint))
	{
		return Color();
		float t = 0.5 * (-ray.direction.y + 1.0);
		return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
	}

	const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-1.0 * hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）
	const Hitable* now_object = objectList[hitpoint.objectId];

	double russianRouletteProb = std::max(now_object->material.color.x, std::max(now_object->material.color.y, now_object->material.color.z));

	// ロシアンルーレット
	if (depth > kDepth)
	{
		if (rand.Next() >= russianRouletteProb)
		{
			return now_object->material.emission;
			//float t = 0.5 * (-ray.direction.y + 1.0);
			//return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
			//return Color(1, 1, 1);
		}
	}
	else
	{
		russianRouletteProb = 1.0;
	}


	//return hitpoint.normal * 0.5 + Color(0.5, 0.5, 0.5);

	// orienting_normalの方向を基準とした正規直交基底(w, u, v)を作る。この基底に対する半球内で次のレイを飛ばす。
	Vector3 w, u, v;
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

	double phi = 2.0 * PI * rand.Next();
	double r2 = rand.Next(), theta = sqrt(r2);
	Vector3 dir = Normalize(
		u * cos(phi) * theta + 
		v * sin(phi) * theta +
		w * sqrt(1.0 - r2)
	);

	Color incoming_radiance = color(Ray(hitpoint.position, dir), objectList, rand, depth + 1) ;
	Color weight =now_object->material.color / russianRouletteProb;
	//return weight * incoming_radiance;
	return now_object->material.emission + weight * incoming_radiance;

	//return color(Ray(hitpoint.position, dir), objectList, rand, depth + 1) / russianRouletteProb;

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
	const double aspectRatio = double(width) / double(height);

	const uint32_t superSampleNum = 8;

	const Vector3 camPos(0, 0, -10);
	const Vector3 targetPos(0, 0, 0);
	const double focalLength = 80.0;	// mm
	const double focalPlane = 30.0;		// mm
	const auto camDir = Normalize(targetPos - camPos);
	//Camera camera(camPos, camDir, focalLength, focalPlane);

	const Vector3 camera_position = Vector3(50.0, 52.0, 220.0);
	const Vector3 camera_dir = Normalize(Vector3(0.0, -0.04, -1.0));
	Camera camera(camera_position, camera_dir, focalLength, focalPlane);

	// イメージセンサー
	// ワールド座標系上にスクリーンを配置
	const double screenWidth = camera.GetFocalPlane() * aspectRatio;
	const double screenHeight = camera.GetFocalPlane();

	const Vector3 screenX = camera.GetRightDirection() * screenWidth;
	const Vector3 screenY = camera.GetUpDirection() * screenHeight;
	const Vector3 screenCenter = camera.GetPosition() + camera.GetDirection() * camera.GetFocalLength();

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

	//#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
	for (auto y = 0; y < height; ++y)
	{
		Random random(y + 1);
		for (auto x = 0; x < width; ++x)
		{
			Color accumlator = Color();
			for (auto sy = 0; sy < superSampleNum; ++sy)
			{
				for (auto sx = 0; sx < superSampleNum; ++sx)
				{
					const double ratio = (1.0 / superSampleNum);
					const double rx = sx * ratio + ratio / 2.0;	// (サブピクセル位置) + (サブピクセル中心へのオフセット)
					const double ry = sy * ratio + ratio / 2.0;

					// -0.5は中心基準になっているため
					Vector3 screenPos = screenCenter +
						(screenX * (((double)x + rx) / width - 0.5)) +
						(screenY * (((double)y + ry) / height - 0.5));
					Vector3 rayDir = Normalize(screenPos - camPos);

					Ray ray = Ray(camPos, rayDir);

					Color col = color(ray, scene, random, 0);
					accumlator += (col / (superSampleNum * superSampleNum));
				}
			}
			ofs << ToInt(accumlator.x) << " " << ToInt(accumlator.y) << " " << ToInt(accumlator.z) << "\n";
		}
	}

	ofs.close();

	std::cout << "end" << std::endl;
	return 0;
}