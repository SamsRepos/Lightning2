#pragma once

struct MyFloat3
{
	float x;
	float y;
	float z;

	MyFloat3()
	{
		x = y = z = 0.f;
	}

	MyFloat3(float _x, float _y, float _z)
	{
		x = _x;
		y = _y;
		z = _z;
	}

	float Magnitude();
	MyFloat3 Normalised();
};


MyFloat3 CrossProduct(const MyFloat3& u, const MyFloat3& v);
float DotProduct(const MyFloat3& u, const MyFloat3& v);

MyFloat3 operator+(const MyFloat3& a, const MyFloat3& b);
MyFloat3 operator-(const MyFloat3& minuend, const MyFloat3& subtrahend);

MyFloat3 operator*(const MyFloat3& vector, const float scale);
MyFloat3 operator/(const MyFloat3& vector, const float denominator);

#define DEFAULT_RANDOM_VECTOR_RESOLUTION 1000
MyFloat3 RandomNormalisedVector(int resolution);
MyFloat3 RandomNormalisedVector();

MyFloat3 RandomPerpendicularUnitVector(const MyFloat3& inputVec);
