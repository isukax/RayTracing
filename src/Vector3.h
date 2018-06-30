#pragma once

#include <cmath>

class Vector3
{
public:
	Vector3();
	Vector3(const double x, const double y, const double z);
	//Vector3(Vector3& vec);

	inline const Vector3 operator+(const Vector3& b) const 
	{
		return Vector3(x + b.x, y + b.y, z + b.z);
	}

	inline const Vector3 operator-(const Vector3& b) const
	{
		return Vector3(x - b.x, y - b.y, z - b.z);
	}

	inline const Vector3 operator-() const
	{
		return Vector3(-x, -y, -z);
	}

	inline const Vector3 operator*(const Vector3& b) const
	{
		return Vector3(x * b.x, y * b.y, z * b.z);
	}

	inline const Vector3 operator*(const double b) const 
	{
		return Vector3(x * b, y * b, z * b);
	}

	inline const Vector3 operator/(const Vector3& b) const
	{
		return Vector3(x / b.x, y / b.y, z / b.z);
	}

	inline const Vector3 operator/(const double b) const
	{
		return Vector3(x / b, y / b, z / b);
	}

	inline Vector3& operator=(const Vector3& b)
	{
		x = b.x;
		y = b.y;
		z = b.z;
		return *this;
	}

	inline Vector3& operator+=(const Vector3& b)
	{
		x += b.x;
		y += b.y;
		z += b.z;
		return *this;
	}

	inline double LengthSquared() const
	{
		return x * x + y * y + z * z;
	}

	inline double Length() const
	{
		return sqrt(LengthSquared());
	}



	double x;
	double y;
	double z;

};

//inline const Vector3 operator*(const double b, const Vector3& a)
//{
//	return Vector3(a.x * b, a.y * b, a.z * b);
//}
inline Vector3 operator*(double b, const Vector3 &v) {
	return v * b;
}

inline Vector3 Normalize(const Vector3& b)
{
	return b / b.Length();
}

inline double Dot(const Vector3& a, const Vector3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline Vector3 Cross(const Vector3& a, const Vector3& b)
{
	return Vector3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}
