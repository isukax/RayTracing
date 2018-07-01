#include <iostream>
#include <algorithm>
#include <direct.h>
#include "Vector3.h"
#include "Ray.h"
#include "Sphere.h"
#include "Scene.h"
#include "Camera.h"
#include "Random.h"
#include "Material.h"
#include "HitPoint.h"
#include "Ppm.h"

// namespace
Color color(Ray& ray, std::vector<Hitable*>& objectList, Random& rand, uint32_t depth)
{
	HitPoint hitpoint;

	if (!IntersectScene(ray, objectList, hitpoint))
	{
		return Color();
	}
	const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）
	Hitable* now_object = objectList[hitpoint.objectId];
	// 色の反射率最大のものを得る。ロシアンルーレットで使う。
	// ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い。
	double russianRouletteProb = std::max(now_object->material->albedo.x, std::max(now_object->material->albedo.y, now_object->material->albedo.z));

	// 反射回数が一定以上になったらロシアンルーレットの確率を急上昇させる。（スタックオーバーフロー対策）
	if (depth > kDepthLimit)
	{
		russianRouletteProb *= pow(0.5, depth - kDepthLimit);
	}

	// ロシアンルーレットを実行し追跡を打ち切るかどうかを判断する。
	// ただしDepth回の追跡は保障する。
	if (depth > kDepth)
	{
		if (rand.Next() >= russianRouletteProb)
		{
			return now_object->material->emission;
			//float t = 0.5 * (-ray.direction.y + 1.0);
			//return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
		}
	}
	else
	{
		russianRouletteProb = 1.0;
	}

	Vector3 dir;
	Color albedo;
	now_object->material->GetRadiance(ray, hitpoint, rand, dir, albedo);
	Color incomingRadiance = color(Ray(hitpoint.position, dir), objectList, rand, depth + 1) ;
	Color weight = now_object->material->albedo / russianRouletteProb;

	// レンダリング方程式に対するモンテカルロ積分を考えると、outgoing_radiance = weight * incoming_radiance。
	// ここで、weight = (ρ/π) * cosθ / pdf(ω) / R になる。
	// ρ/πは完全拡散面のBRDFでρは反射率、cosθはレンダリング方程式におけるコサイン項、pdf(ω)はサンプリング方向についての確率密度関数。
	// Rはロシアンルーレットの確率。
	// 今、コサイン項に比例した確率密度関数によるサンプリングを行っているため、pdf(ω) = cosθ/π
	// よって、weight = ρ/ R。

	return now_object->material->emission + weight * incomingRadiance;
}


int main(int argc, char** argv)
{
	const uint32_t width = 320;//1024;
	const uint32_t height = 240;// 768;
	const double aspectRatio = double(width) / double(height);

	const uint32_t superSampleNum = 2;
	const uint32_t subPixelSampleNum = 8;
	const double focalLength = 40.0 * 1e-4;	// mm
	const double focalPlane = 30.0 * 1e-4;	// mm

	//const Vector3 camPos(0, 0, -10);
	//const Vector3 targetPos(0, 0, 0);
	//const auto camDir = Normalize(targetPos - camPos);
	//Camera camera(camPos, camDir, focalLength, focalPlane);

	const Vector3 camPos = Vector3(50.0, 52.0, 220.0);
	const Vector3 camDir = Normalize(Vector3(0.0, -0.04, -1.0));
	Camera camera(camPos, camDir, focalLength, focalPlane);

	// イメージセンサー
	// ワールド座標系上にスクリーンを配置
	const double screenWidth = camera.GetFocalPlane() * aspectRatio;
	const double screenHeight = camera.GetFocalPlane();

	const Vector3 screenX = camera.GetRightDirection() * screenWidth;
	const Vector3 screenY = camera.GetUpDirection() * screenHeight;
	const Vector3 screenCenter = camera.GetPosition() + camera.GetDirection() * camera.GetFocalLength();

	// Monte Carlo
	// Biased Unbiased
	// Super Sampling
	// Importance Sampling
	// Russian roulette
	// Bidirectional Path tracing
	// BVH KDTREE

	//std::ofstream ofs("result.ppm");
	Color *image = new Color[width * height];
	std::cout << width << "x" << height << " " << subPixelSampleNum * (superSampleNum * superSampleNum) << " spp" << std::endl;

#pragma omp parallel for schedule(dynamic, 1) num_threads(4)
	for (auto y = 0; y < height; ++y)
	{
		std::cerr << "Rendering (y = " << y << ") " << (100.0 * y / (height - 1)) << "%" << std::endl;
		Random random(y + 1);
		for (auto x = 0; x < width; ++x)
		{
			const int image_index = (height - y - 1) * width + x;
			Color accumlator = Color();
			for (auto sy = 0; sy < superSampleNum; ++sy)
			{
				for (auto sx = 0; sx < superSampleNum; ++sx)
				{
					// 一つのサブピクセルあたり複数回サンプリングする
					for (int s = 0; s < subPixelSampleNum; s++) {

						const double ratio = (1.0 / superSampleNum);
						const double rx = sx * ratio + ratio / 2.0;	// (サブピクセル位置) + (サブピクセル中心へのオフセット)
						const double ry = sy * ratio + ratio / 2.0;

						// -0.5は中心基準になっているため
						Vector3 screenPos = screenCenter +
							(screenX * ((x + rx) / width - 0.5)) +
							(screenY * ((y + ry) / height - 0.5));

						Vector3 rayDir = Normalize(screenPos - camPos);
						Ray ray = Ray(camPos, rayDir);

						//Color col = radiance(ray, scene, random, 0);
						Color col = color(ray, scene, random, 0);
						accumlator += (col / subPixelSampleNum / (superSampleNum * superSampleNum));
					}
				}
			}
			image[image_index] += accumlator;
		}
	}

	Ppm::Save(std::string("image/result.ppm"), image, width, height);
	delete[] image;

	return 0;
}
