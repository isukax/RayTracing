#pragma once

#include <memory>
#include "Shape.h"
#include "Model.h"

class ShapeBuilder
{
public:
	ShapeBuilder() {}
	ShapeBuilder(const HitablePtr ptr)
	{
		object = ptr;
	}

	ShapeBuilder& sphere(const Vector3& pos, const double radius, MaterialPtr material)
	{
		object = std::make_shared<Sphere>(pos, radius, material);
		return *this;
	}

	ShapeBuilder& rect(const double w0, const double w1, const double h0, const double h1, const double k, const Rect::AxisType type, MaterialPtr material)
	{
		object = std::make_shared<Rect>(w0, w1, h0, h1, k, type, material);
		return *this;
	}

	ShapeBuilder& box(const Vector3& topLeft, const Vector3& bottomRight, const MaterialPtr material)
	{
		object = std::make_shared<Box>(topLeft, bottomRight, material);
		return *this;
	}

	ShapeBuilder& model(std::string path, const MaterialPtr material)
	{
		object = std::make_shared<Model>(path, material);
		return *this;
	}

	ShapeBuilder& translate(const Vector3& offset)
	{
		object = std::make_shared<Translate>(object, offset);
		return *this;
	}

	ShapeBuilder& rotate(const Vector3& axis, const double radian)
	{
		object = std::make_shared<Rotate>(object, axis, radian);
		return *this;
	}

	ShapeBuilder& scale(const Vector3& scale)
	{
		object = std::make_shared<Scale>(object, scale);
		return *this;
	}

	HitablePtr get()
	{
		return object;
	}

private:
	HitablePtr object;
};