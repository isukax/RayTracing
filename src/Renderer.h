#pragma once
#include <iostream>
#include <cstdint>
#include "Scene.h"
#include "File.h"

//class RenderTarget
//{
//public:
//	RenderTarget(uint32_t width, uint32_t height)
//		: width(width)
//		, height(height)
//	{
//		image = new Color[width * height];
//	}
//
//	~RenderTarget()
//	{
//		delete[] image;
//	}
//
//private:
//	Color* image;
//	uint32_t width;
//	uint32_t height;
//};

class Renderer
{
public:
	Renderer(int width, int height, int subPixelSampleNum, int superSampleNum)
		: width(width)
		, height(height)
		, subPixelSampleNum(subPixelSampleNum)
		, superSampleNum(superSampleNum)
	{
	}

	void SetScene(ScenePtr scene) { this->scene = scene; }

	void Render()
	{
		auto camera = scene->GetCamera();
		const double aspectRatio = double(width) / double(height);

		// イメージセンサー
		// ワールド座標系上にスクリーンを配置
		const double screenWidth = camera->GetFocalPlane() * aspectRatio;
		const double screenHeight = camera->GetFocalPlane();

		const Vector3 screenX = camera->GetRightDirection() * screenWidth;
		const Vector3 screenY = camera->GetUpDirection() * screenHeight;
		const Vector3 screenCenter = camera->GetPosition() + camera->GetDirection() * camera->GetFocalLength();

		Color *image = new Color[width * height];
		std::cout << width << "x" << height << " " << subPixelSampleNum * (superSampleNum * superSampleNum) << " spp" << std::endl;

#pragma omp parallel for schedule(dynamic, 1) num_threads(8)
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

							Vector3 rayDir = Normalize(screenPos - camera->GetPosition());
							Ray ray = Ray(camera->GetPosition(), rayDir);

							//Color col = radiance(ray, scene, random, 0);
							Color col = radiance(ray, random, 0);
							accumlator += (col / subPixelSampleNum / (superSampleNum * superSampleNum));
						}
					}
				}
				image[image_index] += accumlator;
			}
		}

		File::Save(std::string("result"), image, width, height);
		delete[] image;
	}

private:
	Color radiance(Ray& ray, Random& rand, uint32_t depth)
	{
		HitPoint hitpoint;
		if (!scene->Intersect(ray, hitpoint))
		{
			auto& iblTexture = scene->GetIblTexture();
			if (iblTexture == nullptr) return Color();
			double phi = atan2(ray.direction.z, ray.direction.x);
			//double phi = atan2(ray.direction.x, ray.direction.z);
			double theta = asin(ray.direction.y);
			double u = 1.0 - (phi + kPI) / kPI2;
			double v = (theta + kPI / 2.0) / kPI;
			return iblTexture->value(u, v, hitpoint.normal);
			//float t = 0.5 * (-ray.direction.y + 1.0);
			//return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
			//return Color();
		}
		const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // 交差位置の法線（物体からのレイの入出を考慮）
		HitablePtr hitObject = hitpoint.object;

		// 色の反射率最大のものを得る。ロシアンルーレットで使う。
		// ロシアンルーレットの閾値は任意だが色の反射率等を使うとより良い。
		auto& albedo = hitObject->GetMaterial()->albedo->value(hitpoint.u, hitpoint.v, hitpoint.position);
		double russianRouletteProb = std::max(albedo.x, std::max(albedo.y, albedo.z));

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
				return hitObject->GetMaterial()->emission;
				//float t = 0.5 * (-ray.direction.y + 1.0);
				//return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
			}
		}
		else
		{
			russianRouletteProb = 1.0;
		}

		return hitObject->GetMaterial()->GetRadiance(ray, hitpoint, rand, depth, russianRouletteProb, [&](Vector3 dir, Color weight)
		{
			Color incomingRadiance = radiance(Ray(hitpoint.position, dir), rand, depth + 1);
			//Color weight = now_object->material->albedo / russianRouletteProb;
			return hitObject->GetMaterial()->emission + weight * incomingRadiance;
		});

		//Color incomingRadiance = color(Ray(hitpoint.position, dir), objectList, rand, depth + 1) ;
		//Color weight = now_object->material->albedo / russianRouletteProb;

		// レンダリング方程式に対するモンテカルロ積分を考えると、outgoing_radiance = weight * incoming_radiance。
		// ここで、weight = (ρ/π) * cosθ / pdf(ω) / R になる。
		// ρ/πは完全拡散面のBRDFでρは反射率、cosθはレンダリング方程式におけるコサイン項、pdf(ω)はサンプリング方向についての確率密度関数。
		// Rはロシアンルーレットの確率。
		// 今、コサイン項に比例した確率密度関数によるサンプリングを行っているため、pdf(ω) = cosθ/π
		// よって、weight = ρ/ R。

		//return now_object->material->emission + weight * incomingRadiance;
	}

private:
	int width;
	int height;
	int subPixelSampleNum;
	int superSampleNum;
	
	ScenePtr scene;
};