#pragma once

#include "Vector3.h"

Vector3 RandomInUnitDisk(Random& rand) {
	Vector3 p;
	do {
		p = 2.0 * Vector3(rand.Next(), rand.Next(), 0) - Vector3(1, 1, 0);
	} while (Dot(p, p) >= 1.0);
	return p;
}

double DegreeToRadian(double degree)
{
	return degree * kPI / 180.0;
}

double determinant(const Vector3& a, const Vector3& b, const Vector3& c)
{
	return (a.x * b.y * c.z) + (a.y * b.z * c.x) + (a.z * b.x * c.y) - (a.x * b.z * c.y) - (a.y * b.x * c.z) - (a.z * b.y * c.x);
}