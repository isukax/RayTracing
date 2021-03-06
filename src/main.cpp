﻿#include <chrono>
#include <algorithm>
#include <direct.h>
#include "Renderer.h"
#include "ShapeBuilder.h"
#include "Model.h"
#include "Util.h"

ScenePtr SceneIbl()
{
	double base = 555;
	const Vector3 camPos = Vector3(base / 2.0, base / 2.0, -500);
	const Vector3 camDir = Normalize(Vector3(0.0, 0, 1.0));
	const double focalLength = 15.0 * 1e-3;	// mm
	const double focalPlane = 30.0 * 1e-3;	// mm
	CameraPtr camera = std::make_shared<Camera>(camPos, camDir, focalLength, focalPlane);
	ScenePtr scene = std::make_shared<Scene>(camera);

	scene->SetIblTexture(std::string(RESOURCE_DIR) + "ibl/Tokyo_BigSight_3k.hdr");

	ShapeBuilder builder;
	//Model model(std::string(MODEL_DIR) + "bunny.obj", std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.25, 0.25))));
	std::vector<HitablePtr> list
	{
		std::make_shared<Rect>(-base * 2, base * 4, 0, base * 2, -60,		Rect::AxisType::XZ,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))),				// 天井
		std::make_shared<Sphere>(Vector3(base / 2 - 650, base / 2, base / 2),			300,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.99)), // 鏡
		std::make_shared<Sphere>(Vector3(base / 2 + 650, base/2, base / 2),			300,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.5)), // 鏡
		std::make_shared<Sphere>(Vector3(base / 2, base / 2, base / 2),			300,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
		//std::make_shared<Model>(std::string(MODEL_DIR) + "bunny.obj", std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.25, 0.25)))),				// 奥
		//std::make_shared<Rect>(0, base, 0, base, base,	Rect::AxisType::XY,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)),				// 奥
		//std::make_shared<Triangle>(Vector3(0, 0, base - 1), Vector3(base, base, base - 1), Vector3(base, 0, base - 1),	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
		//std::make_shared<Triangle>(Vector3(0, base, base-1), Vector3(base, base, base-1), Vector3(0, 0, base-1),	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
	};
	scene->Add(list);

	return scene;
}

ScenePtr SceneUVSphere()
{
	const Vector3 camPos = Vector3(50.0, 52.0, 220.0);
	const Vector3 camDir = Normalize(Vector3(0.0, -0.04, -1.0));
	const double focalLength = 40.0 * 1e-3;	// mm
	const double focalPlane = 30.0 * 1e-3;	// mm
	CameraPtr camera = std::make_shared<Camera>(camPos, camDir, focalLength, focalPlane);
	ScenePtr scene = std::make_shared<Scene>(camera);

	std::vector<HitablePtr> list
	{
		std::make_shared<Sphere>(Vector3(1e5 + 1, 40.8, 81.6),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.25, 0.25)))),	// 左
		std::make_shared<Sphere>(Vector3(-1e5 + 99, 40.8, 81.6),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.25, 0.75)))),	// 右
		std::make_shared<Sphere>(Vector3(50, 40.8, 1e5),			1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))),	// 奥
		std::make_shared<Sphere>(Vector3(50, 40.8, -1e5 + 250),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()))),					// 手前
		std::make_shared<Sphere>(Vector3(50, 1e5, 81.6),			1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))),	// 床
		//std::make_shared<Sphere>(Vector3(50, 1e5, 81.6),			1e5,	std::make_shared<LambertMaterial>(std::make_shared<CheckerTexture>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)), std::make_shared<ColorTexture>(Color(0, 0, 0)), .5))),	// 床
	
		std::make_shared<Sphere>(Vector3(50, -1e5 + 81.6, 81.6),	1e5,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))),	// 天井
		std::make_shared<Sphere>(Vector3(65, 20, 78),				20,		std::make_shared<LambertMaterial>(std::make_shared<ImageTexture>(std::string(RESOURCE_DIR) + "uv2.jpg"))), // UV球
		//std::make_shared<Rect>(20, 50, 0, 30, 10, Rect::AxisType::XY,	std::make_shared<LambertMaterial>(std::make_shared<CheckerTexture>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)), std::make_shared<ColorTexture>(Color(0, 0, 0)), .5), Color(10, 10, 10))), // 四角テクスチャ
		//std::make_shared<Rect>(20, 50, 0, 30, 10, Rect::AxisType::XY,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))), // 四角
		//std::make_shared<Sphere>(Vector3(65, 20, 20),				20,		std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.75, 0.25)))), // 緑球
		//std::make_shared<Sphere>(Vector3(27, 16.5, 47),			16.5,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
		//std::make_shared<Sphere>(Vector3(77, 16.5, 78),			16.5,	std::make_shared<DielectricMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)))), //ガラス
		std::make_shared<Sphere>(Vector3(50.0, 90.0, 81.6),		15.0,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()), Color(36,36,36))), //照明
	};
	scene->Add(list);
	return scene;
}

ScenePtr SceneCornelBox()
{
	double base = 555;
	const Vector3 camPos = Vector3(base / 2.0, base / 2.0, -600);
	const Vector3 camDir = Normalize(Vector3(0.0, 0, 1.0));
	const double focalLength = 30.0 * 1e-3;	// mm
	const double focalPlane = 30.0 * 1e-3;	// mm
	CameraPtr camera = std::make_shared<Camera>(camPos, camDir, focalLength, focalPlane);
	ScenePtr scene = std::make_shared<Scene>(camera);

	scene->SetIblTexture("ibl/Tokyo_BigSight_3k.hdr");
	//scene->SetIblTexture("Tropical_Beach_3k.hdr");
	//scene->SetIblTexture("Playa_Sunrise_8k.jpg");

	//auto slist = std::make_shared<ShapeList>();
	//slist->Add(std::make_shared<Sphere>(Vector3(base / 2.0, base / 2.0, base / 2.0), 20, std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.75, 0.25)))));
	//slist->Add(std::make_shared<Sphere>(Vector3(base / 2.0, base / 2.0 + 100, base / 2.0), 20, std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.75, 0.75)))));
	//slist->Add(std::make_shared<Sphere>(Vector3(base / 2.0, base / 2.0 + 100, base / 2.0), 50.0, std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()), Color(36, 36, 36))));
	ShapeBuilder builder;
	std::vector<HitablePtr> list
	{
		std::make_shared<Rect>(0, base, 0, base, base,	Rect::AxisType::XY,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))),				// 奥
		//std::make_shared<Rect>(0, base, 0, base, 0,		Rect::AxisType::XY,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()))),						// 手前
		std::make_shared<Rect>(0, base, 0, base, 0,		Rect::AxisType::YZ,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.25, 0.25)))),			// 左
		std::make_shared<Rect>(0, base, 0, base, base,	Rect::AxisType::YZ,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.25, 0.75)))),			// 右
		std::make_shared<Rect>(0, base, 0, base, 0,		Rect::AxisType::XZ,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))),				// 天井
		std::make_shared<Rect>(0, base, 0, base, base, Rect::AxisType::XZ,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))),				// 床
		std::make_shared<Sphere>(Vector3(base / 2.0, base, base / 2.0),		50.0,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color()), Color(36,36,36))),	//照明

		//std::make_shared<Sphere>(Vector3(base / 2.0, base / 2.0, base / 2.0),				20,		std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.25, 0.75, 0.25)))), // 緑球
		//std::make_shared<Sphere>(Vector3(base/2, 200, base/2),				100,		std::make_shared<LambertMaterial>(std::make_shared<ImageTexture>("uv2.jpg"))), // UV球
		//std::make_shared<Box>(Vector3(130, 0, 165), Vector3(295, 165, 330), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))),
		//std::make_shared<Translate>(std::make_shared<Box>(Vector3(265, 0, 295), Vector3(430, 330, 460), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))), Vector3(100, 0, 0))
		builder.box(Vector3(265, 0, 295), Vector3(430, 330, 460), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(1, 0, 0), DegreeToRadian(0)).get(),
		builder.box(Vector3(265, 0, 295), Vector3(430, 330, 460), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(1, 0, 0), DegreeToRadian(10)).get(),

		//builder.model(std::string(MODEL_DIR) + "bunny.obj", std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)).rotate(Vector3(0, 1, 0), DegreeToRadian(90)).translate(Vector3(100, 0, 0)).get()
	};
	scene->Add(list);

	return scene;
}

ScenePtr SceneBunny()
{
	const Vector3 camPos = Vector3(0, 0, -50);
	const Vector3 camDir = Normalize(Vector3(0.0, 0, 1.0));
	const double focalLength = 20.0 * 1e-3;	// mm
	const double focalPlane = 35.0 * 1e-3;	// mm
	CameraPtr camera = std::make_shared<Camera>(camPos, camDir, focalLength, focalPlane);
	ScenePtr scene = std::make_shared<Scene>(camera);

	scene->SetIblTexture(std::string(RESOURCE_DIR) + "ibl/Tokyo_BigSight_3k.hdr");

	double radius = 10;
	double offset = 20;
	ShapeBuilder builder;
	std::vector<HitablePtr> list
	{
		//std::make_shared<Rect>(-100, 100, 0, 10, 050,		Rect::AxisType::XZ,	std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))),				// 天井
		//std::make_shared<Sphere>(Vector3(-offset, 0, 0),	radius,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.99)), // 鏡
		//std::make_shared<Sphere>(Vector3(offset, 0, 0),		radius,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.5)), // 鏡
		//std::make_shared<Sphere>(Vector3(0, 0, 0),		radius,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
		//std::make_shared<Model>(std::string(MODEL_DIR) + "bunny.obj", std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(0.75, 0.25, 0.25)))),				// 奥
		//builder.model(std::string(MODEL_DIR) + "duck.fbx",std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.1)).scale(Vector3(2, 2, 2)).translate(Vector3(0, 0, 0)).get(),
		//builder.model(std::string(MODEL_DIR) + "duck.fbx",std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.1)).scale(Vector3(0.2, 0.2, 0.2)).rotate(Vector3(0, 1, 0), DegreeToRadian(70)).rotate(Vector3(1, 0, 0), DegreeToRadian(-30)).translate(Vector3(0, -10, 0)).get(),
		builder.model(std::string(MODEL_DIR) + "bunny.obj", std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0.1)).scale(Vector3(100, 100, 100)).rotate(Vector3(0, 1, 0), DegreeToRadian(-15)).translate(Vector3(2.5, -10, -30)).get(),
		//builder.model(std::string(MODEL_DIR) + "bunny.obj", std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(0, 1, 0), DegreeToRadian(-15)).translate(Vector3(2.5, -10, -30)).get(),
		//builder.sphere(Vector3(10, 0, 0), 20, std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)).rotate(Vector3(0, 1, 0), DegreeToRadian(40)).get(),
		//builder.box(Vector3(-10, -20, -10), Vector3(10, 20, 10), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).get()//.rotate(Vector3(0, 1, 0), DegreeToRadian(40)).get(),
		//builder.box(Vector3(-10, -20, -10), Vector3(10, 20, 10), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(0, 1, 0), DegreeToRadian(180)).get(),
		//builder.box(Vector3(0, 0, 0), Vector3(10, 20, 10), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(0, 1, 0), DegreeToRadian(-45)).get(),
		//builder.box(Vector3(0, 0, 0), Vector3(10, 20, 10), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(0, 1, 0), DegreeToRadian(0)).get(),

		//builder.box(Vector3(0, 0, 0), Vector3(10, 20, 10), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(0, 0, 1), DegreeToRadian(45)).get(),
		//builder.box(Vector3(0, 0, 0), Vector3(10, 20, 10), std::make_shared<LambertMaterial>(std::make_shared<ColorTexture>(Color(.75, .75, .75)))).rotate(Vector3(0, 0, 1), DegreeToRadian(0)).get(),
		//std::make_shared<Rect>(0, base, 0, base, base,	Rect::AxisType::XY,	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)),				// 奥
		//std::make_shared<Triangle>(Vector3(0, 0, base - 1), Vector3(base, base, base - 1), Vector3(base, 0, base - 1),	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
		//std::make_shared<Triangle>(Vector3(0, base, base-1), Vector3(base, base, base-1), Vector3(0, 0, base-1),	std::make_shared<MetalMaterial>(std::make_shared<ColorTexture>(Color(0.99, 0.99, 0.99)), Color(), 0)), // 鏡
	};
	scene->Add(list);

	return scene;
}

// Bidirectional Path tracing
// BVH KDTREE

int main(int argc, char** argv)
{
	std::chrono::system_clock::time_point start, end;

	//Renderer renderer(1024, 1024, 16, 8);
	//Renderer renderer(1024, 1024, 8, 2);
	Renderer renderer(320, 240, 8, 1);
	//Renderer renderer(640, 480, 4, 8);
	//const double cocSize = 0.03328 * 1e-3;	//mm
	//const double aperture = 1;
	//const double FNumber = focalLength / aperture;
	//double forwardDoF = (cocSize * FNumber *   

	//const Vector3 camPos(0, 0, -10);
	//const Vector3 targetPos(0, 0, 0);
	//const auto camDir = Normalize(targetPos - camPos);

	ScenePtr scene = SceneBunny();
	auto camera = scene->GetCamera();
	renderer.SetScene(scene);

	start = std::chrono::system_clock::now();

	renderer.Render();

	end = std::chrono::system_clock::now();
	
	int64_t elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
	std::cerr << "time : " << elapsed << std::endl;

	//_sleep(5000);

	return 0;
}
