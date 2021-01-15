#include "MyFloat3.h"

#include <math.h>
#include <random>

float magnitude(const MyFloat3& vector)
{
	return sqrt(
		vector.x * vector.x
		+
		vector.y * vector.y
		+
		vector.z * vector.z
	);
}

MyFloat3 normalised(MyFloat3& vector)
{
	return vector / magnitude(vector);
}

MyFloat3 crossProduct(const MyFloat3& u, const MyFloat3& v)
{
	MyFloat3 res;
	res.x = (u.y * v.z) - (u.z * v.y);
	res.y = (u.z * v.x) - (u.x * v.z);
	res.z = (u.x * v.y) - (u.y * v.x);
	return res;
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

MyFloat3 randomNormalisedVector(int resolution)
{
	int halfResolution = resolution / 2;
	MyFloat3 res;
	
	res.x = (rand() % resolution) - halfResolution;
	res.y = (rand() % resolution) - halfResolution;
	res.z = (rand() % resolution) - halfResolution;
		
	return normalised(res);
}

MyFloat3 randomNormalisedVector()
{
	return randomNormalisedVector(DEFAULT_RANDOM_VECTOR_RESOLUTION);
}