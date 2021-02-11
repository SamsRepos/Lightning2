#include "MyMatrix44.h"

#include <math.h>

MyFloat4 operator*(const MyFloat4& vec, const MyMatrix44& mat)
{
	MyFloat4 res;

	res.x = (vec.x * mat.values[0].x) + (vec.y * mat.values[1].x) + (vec.z * mat.values[2].x) + (vec.w * mat.values[3].x);
	res.y = (vec.x * mat.values[0].y) + (vec.y * mat.values[1].y) + (vec.z * mat.values[2].y) + (vec.w * mat.values[3].y);
	res.z = (vec.x * mat.values[0].z) + (vec.y * mat.values[1].z) + (vec.z * mat.values[2].z) + (vec.w * mat.values[3].z);
	res.w = (vec.x * mat.values[0].w) + (vec.y * mat.values[1].w) + (vec.z * mat.values[2].w) + (vec.w * mat.values[3].w);

	return res;
}

MyFloat3 operator*(const MyFloat3& vec, const MyMatrix44& mat)
{
	MyFloat4 res4 = MyFloat4(vec) * mat;
	MyFloat3 res = MyFloat3(res4.x, res4.y, res4.z);

	if (res4.w != 0.f)
	{
		res = res / res4.w;
	}

	return res;
}

MyMatrix44 RotationMatrix(const MyFloat3& axisThroughOrigin, const float& angle)
{
	float alpha = axisThroughOrigin.x;
	float beta  = axisThroughOrigin.y;
	float gamma = axisThroughOrigin.z;

	float alphaSquared = alpha * alpha;
	float betaSquared = beta * beta;
	float gammaSquared = gamma * gamma;

	float alphaBeta = alpha * beta;
	float alphaGamma = alpha * gamma;
	float betaGamma = beta * gamma;
		
	float cosTheta = cos(angle);
	float sinTheta = sin(angle);
	
	float rotationValues[] = {
		(alphaSquared * (1 - cosTheta) + cosTheta),      (alphaBeta * (1 - cosTheta) + gamma * sinTheta), (alphaGamma * (1 - cosTheta) - beta * sinTheta), 0.f,
		(alphaBeta * (1 - cosTheta) - gamma * sinTheta), (betaSquared * (1 - cosTheta) + cosTheta),       (betaGamma * (1 - cosTheta) - alpha * sinTheta), 0.f,
		(alphaGamma * (1 - cosTheta) + beta * sinTheta), (betaGamma * (1 - cosTheta) - alpha * sinTheta), (gammaSquared * (1 - cosTheta) + cosTheta),      0.f,
		0.f,                                             0.f,                                             0.f,                                             1.f
	};

	return MyMatrix44(rotationValues);
}