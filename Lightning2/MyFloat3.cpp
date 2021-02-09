#include "MyFloat3.h"

#include <math.h>
#include <random>

float MyFloat3::Magnitude()
{
	return sqrt(
		(x * x) + (y * y) + (z * z)
	);
}

MyFloat3 MyFloat3::Normalised()
{
	return (*this) / Magnitude();
}

MyFloat3 CrossProduct(const MyFloat3& u, const MyFloat3& v)
{
	MyFloat3 res;
	res.x = (u.y * v.z) - (u.z * v.y);
	res.y = (u.z * v.x) - (u.x * v.z);
	res.z = (u.x * v.y) - (u.y * v.x);
	return res;
}

float DotProduct(const MyFloat3& u, const MyFloat3& v)
{
	return (u.x * v.x) + (u.y * v.y) + (u.z * v.z);
}

MyFloat3 operator+(const MyFloat3& a, const MyFloat3& b)
{
	MyFloat3 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

MyFloat3 operator-(const MyFloat3& minuend, const MyFloat3& subtrahend)
{
	MyFloat3 res;
	res.x = minuend.x - subtrahend.x;
	res.y = minuend.y - subtrahend.y;
	res.z = minuend.z - subtrahend.z;
	return res;
}

MyFloat3 operator*(const MyFloat3& vector, const float scale)
{
	MyFloat3 res;
	res.x = vector.x * scale;
	res.y = vector.y * scale;
	res.z = vector.z * scale;
	return res;
}

MyFloat3 operator/(const MyFloat3& vector, const float denominator)
{
	MyFloat3 res;
	res.x = vector.x / denominator;
	res.y = vector.y / denominator;
	res.z = vector.z / denominator;
	return res;
}

MyFloat3 RandomNormalisedVector(int resolution)
{
	int halfResolution = resolution / 2;
	MyFloat3 res;
	
	res.x = (rand() % resolution) - halfResolution;
	res.y = (rand() % resolution) - halfResolution;
	res.z = (rand() % resolution) - halfResolution;
		
	return res.Normalised();
}

MyFloat3 RandomNormalisedVector()
{
	return RandomNormalisedVector(DEFAULT_RANDOM_VECTOR_RESOLUTION);
}

MyFloat3 RandomPerpendicularUnitVector(const MyFloat3& inputVec)
{
	MyFloat3 res = CrossProduct(
		RandomNormalisedVector(),
		inputVec
	);

	return res.Normalised();
}

