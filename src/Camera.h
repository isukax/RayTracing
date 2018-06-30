#pragma once

#include "Vector3.h"

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
		upDirection = Cross(rightDirection, direction);
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
	
	// �œ_�������������Ɖ�p���L���Ȃ�L�͈͂��ʂ�
	double focalLength;	// mm
	double focalPlane;	// mm
};