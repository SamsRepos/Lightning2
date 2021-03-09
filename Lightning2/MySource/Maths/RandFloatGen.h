#pragma once

#include <random>

//a singleton class

class RandFloatGen
{
public:	
	RandFloatGen();

	float GetRandFloat(float max);
	float GetRandFloat(float min, float max);
private:
	const float RESOLUTION = 1000.f;
};

