#pragma once

#include "Vector3.h"

class Camera;
using CameraPtr = std::shared_ptr<Camera>;

class Camera
{
public:
	Camera(Vector3 pos, Vector3 dir, double fl, double fp)
		: position(pos)
		, direction(dir)
		, focalLength(fl)
		, focalPlane(fp)
	{
		Vector3 upDir = Vector3(0, 1, 0);
		rightDirection = Normalize(Cross(upDir, direction));
		upDirection = Cross(direction, rightDirection);
	}

	//void SetFocalLength(double fl) { focalLength = fl; }
	//void SetFocalPlane(double fp) { focalPlane = fp; }

	Vector3 GetPosition() { return position; }
	Vector3 GetDirection() { return direction; }
	Vector3 GetUpDirection() { return upDirection; }
	Vector3 GetRightDirection() { return rightDirection; }
	double GetFocalLength() { return focalLength; }
	double GetFocalPlane() { return focalPlane; }

private:
	Vector3 position;
	Vector3 direction;
	Vector3 upDirection;
	Vector3 rightDirection;
	
	// 焦点距離が小さいと画角が広くなり広範囲を写す
	double focalLength;	// mm
	double focalPlane;	// mm
};