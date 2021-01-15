#include "MyVectorUtils.h"


float magnitude(const XMFLOAT3& vector)
{
	return sqrt(
		vector.x * vector.x
		+
		vector.y * vector.y
		+
		vector.z * vector.z
	);
}

XMFLOAT3 normalised(XMFLOAT3& vector)
{
	return vector / magnitude(vector);
}

XMFLOAT3 crossProduct(const XMFLOAT3& u, const XMFLOAT3& v)
{
	XMFLOAT3 res;
	res.x = (u.y * v.z) - (u.z * v.y);
	res.y = (u.z * v.x) - (u.x * v.z);
	res.z = (u.x * v.y) - (u.y * v.x);
	return res;
}

XMFLOAT3 operator+(const XMFLOAT3& a, const XMFLOAT3& b)
{
	XMFLOAT3 res;
	res.x = a.x + b.x;
	res.y = a.y + b.y;
	res.z = a.z + b.z;
	return res;
}

XMFLOAT3 operator-(const XMFLOAT3& minuend, const XMFLOAT3& subtrahend)
{
	XMFLOAT3 res;
	res.x = minuend.x - subtrahend.x;
	res.y = minuend.y - subtrahend.y;
	res.z = minuend.z - subtrahend.z;
	return res;
}

XMFLOAT3 operator*(const XMFLOAT3& vector, const float scale)
{
	XMFLOAT3 res;
	res.x = vector.x * scale;
	res.y = vector.y * scale;
	res.z = vector.z * scale;
	return res;
}

XMFLOAT3 operator/(const XMFLOAT3& vector, const float denominator)
{
	XMFLOAT3 res;
	res.x = vector.x / denominator;
	res.y = vector.y / denominator;
	res.z = vector.z / denominator;
	return res;
}

XMFLOAT3 randomNormalisedVector(int resolution)
{
	int halfResolution = resolution / 2;
	XMFLOAT3 res = XMFLOAT3(
		(rand() % resolution) - halfResolution,
		(rand() % resolution) - halfResolution,
		(rand() % resolution) - halfResolution
	);
	
	return normalised(res);
}

XMFLOAT3 randomNormalisedVector()
{
	return randomNormalisedVector(DEFAULT_RANDOM_VECTOR_RESOLUTION);
}