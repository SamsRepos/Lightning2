#include "MyLerp.h"

float MyLerp(float a, float b, float t)
{
	return ((1 - t) * a) + (t * b);
}

float MyInverseLerp(float a, float b, float value)
{
	return (value - a) / (b - a);
}

float MyLerpAngleRad(float a, float b, float t)
{
	float angleDiff = b - a;

	if (angleDiff > PI)
	{
		angleDiff -= TWO_PI;
	}
	else if (angleDiff < -PI)
	{
		angleDiff += TWO_PI;
	}

	float angle = a + (t * angleDiff);
	return angle;
}

float MyLerpAngleDeg(float a, float b, float t)
{
	float angleDiff = b - a;

	if (angleDiff > 180.f)
	{
		angleDiff -= 360.f;
	}
	else if (angleDiff < -180.f)
	{
		angleDiff += 360.f;
	}

	float angle = a + (t * angleDiff);

	return angle;
}