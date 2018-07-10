#pragma once

#include <vector>
#include <memory>
#include "Shape.h"
#include "Camera.h"

class Scene;
using ScenePtr = std::shared_ptr<Scene>;

class Scene
{
public:
	Scene(CameraPtr camera)
		: camera(camera)
		, objectList()
		, iblTexture()
	{}

	void Add(HitablePtr object) { objectList.push_back(object); }
	void Add(std::vector<HitablePtr> list) { objectList.insert(objectList.end(), list.begin(), list.end()); }
	void SetIblTexture(std::string fileName)
	{
		iblTexture = std::make_shared<ImageTexture>(fileName);
	}

	const std::vector<HitablePtr>& GetObjectList() const { return objectList; }
	const CameraPtr GetCamera() const { return camera; }
	const ImageTexturePtr GetIblTexture() const { return iblTexture; }

	bool Intersect(Ray& ray, HitPoint& hitpoint)
	{
		HitablePtr hitObject = nullptr;
		for (auto& obj : objectList)
		{
			HitPoint h;
			if (!obj->intersect(ray, h))
			{
				continue;
			}

			if (h.distance < hitpoint.distance)
			{
				hitpoint = h;
				if (h.object == nullptr)
				{
					hitpoint.object = obj;
					//hitpoint.object = h.object;
				}
				hitObject = obj;
			}
		}

		if(hitObject == nullptr)
		{
			return false;
		}
		return true;
	}

private:
	CameraPtr camera;
	std::vector<HitablePtr> objectList;
	ImageTexturePtr iblTexture;

};