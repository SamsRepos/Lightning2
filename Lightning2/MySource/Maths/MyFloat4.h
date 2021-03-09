#pragma once

#include "MyFloat3.h"

struct MyFloat4
{
	float x;
	float y;
	float z;
	float w;

	MyFloat4()
	{
		x = y = z = 0.f;
		w = 1.f;
	}

	MyFloat4(float _x, float _y, float _z, float _w = 1.f)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	MyFloat4(MyFloat3 float3, float _w = 1.f)
	{
		x = float3.x;
		y = float3.y;
		z = float3.z;
		w = _w;
	}
};