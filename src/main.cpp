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
Color color(Ray& ray, std::vector<Hitable*>& objectList, Random& rand, uint32_t depth)
{
	HitPoint hitpoint;

	if (!IntersectScene(ray, objectList, hitpoint))
	{
		return Color();
	}
	const Vector3 normal = Dot(hitpoint.normal, ray.direction) < 0.0 ? hitpoint.normal : (-hitpoint.normal); // �����ʒu�̖@���i���̂���̃��C�̓��o���l���j
	const Hitable* now_object = objectList[hitpoint.objectId];

	double russianRouletteProb = std::max(now_object->material.color.x, std::max(now_object->material.color.y, now_object->material.color.z));

	// ���ˉ񐔂����ȏ�ɂȂ����烍�V�A�����[���b�g�̊m�����}�㏸������B�i�X�^�b�N�I�[�o�[�t���[�΍�j
	if (depth > kDepthLimit)
	{
		russianRouletteProb *= pow(0.5, depth - kDepthLimit);
	}

	// ���V�A�����[���b�g�����s���ǐՂ�ł��؂邩�ǂ����𔻒f����B
	// ������Depth��̒ǐՂ͕ۏႷ��B
	if (depth > kDepth)
	{
		if (rand.Next() >= russianRouletteProb)
		{
			return now_object->material.emission;
			//float t = 0.5 * (-ray.direction.y + 1.0);
			//return Color(1.0, 1.0, 1.0) * (1.0 - t) + Color(0.5, 0.7, 1.0) * t;
		}
	}
	else
	{
		russianRouletteProb = 1.0;
	}

	// orienting_normal�̕�������Ƃ������K�������(w, u, v)�����B���̊��ɑ΂��锼�����Ŏ��̃��C���΂��B
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

	Color incomingRadiance = color(Ray(hitpoint.position, dir), objectList, rand, depth + 1) ;
	Color weight = now_object->material.color / russianRouletteProb;

	// �����_�����O�������ɑ΂��郂���e�J�����ϕ����l����ƁAoutgoing_radiance = weight * incoming_radiance�B
	// �����ŁAweight = (��/��) * cos�� / pdf(��) / R �ɂȂ�B
	// ��/�΂͊��S�g�U�ʂ�BRDF�Ńς͔��˗��Acos�Ƃ̓����_�����O�������ɂ�����R�T�C�����Apdf(��)�̓T���v�����O�����ɂ��Ă̊m�����x�֐��B
	// R�̓��V�A�����[���b�g�̊m���B
	// ���A�R�T�C�����ɔ�Ⴕ���m�����x�֐��ɂ��T���v�����O���s���Ă��邽�߁Apdf(��) = cos��/��
	// ����āAweight = ��/ R�B

	return now_object->material.emission + weight * incomingRadiance;
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

void save_ppm_file(const std::string &filename, const Color *image, const int width, const int height) {
	FILE *f = fopen(filename.c_str(), "wb");
	fprintf(f, "P3\n%d %d\n%d\n", width, height, 255);
	for (int i = 0; i < width * height; i++)
		fprintf(f, "%d %d %d ", ToInt(image[i].x), ToInt(image[i].y), ToInt(image[i].z));
	fclose(f);
}

int main(int argc, char** argv)
{
	//render(320, 240, 8, 2);	return 0;;

	const uint32_t width = 320;//1024;
	const uint32_t height = 240;// 768;
	const double aspectRatio = double(width) / double(height);

	const uint32_t superSampleNum = 2;
	const uint32_t subPixelSampleNum = 8;
	const double focalLength = 40.0;	// mm
	const double focalPlane = 30.0;		// mm

	//const Vector3 camPos(0, 0, -10);
	//const Vector3 targetPos(0, 0, 0);
	//const auto camDir = Normalize(targetPos - camPos);
	//Camera camera(camPos, camDir, focalLength, focalPlane);

	const Vector3 camPos = Vector3(50.0, 52.0, 220.0);
	const Vector3 camDir = Normalize(Vector3(0.0, -0.04, -1.0));
	Camera camera(camPos, camDir, focalLength, focalPlane);

	// �C���[�W�Z���T�[
	// ���[���h���W�n��ɃX�N���[����z�u
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
					// ��̃T�u�s�N�Z�������蕡����T���v�����O����
					for (int s = 0; s < subPixelSampleNum; s++) {

						const double ratio = (1.0 / superSampleNum);
						const double rx = sx * ratio + ratio / 2.0;	// (�T�u�s�N�Z���ʒu) + (�T�u�s�N�Z�����S�ւ̃I�t�Z�b�g)
						const double ry = sy * ratio + ratio / 2.0;

						// -0.5�͒��S��ɂȂ��Ă��邽��
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

	save_ppm_file(std::string("result.ppm"), image, width, height);
	delete[] image;

	return 0;
}
