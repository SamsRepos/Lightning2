#pragma once

#include "MyFloat4.h"

struct MyMatrix44
{
	MyFloat4 values[4];

	MyMatrix44(const float *m)
	{
		values[0] = MyFloat4(m[0], m[1], m[2], m[3]);
		values[1] = MyFloat4(m[4], m[5], m[6], m[7]);
		values[2] = MyFloat4(m[8], m[9], m[10], m[11]);
		values[3] = MyFloat4(m[12], m[13], m[14], m[15]);
	}
};

MyFloat4 operator*(const MyFloat4& vec, const MyMatrix44& mat);
MyFloat3 operator*(const MyFloat3& vec, const MyMatrix44& mat);