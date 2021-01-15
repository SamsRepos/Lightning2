#include "RandFloatGen.h"

#include <cassert>

RandFloatGen::RandFloatGen()
{
}

float RandFloatGen::GetRandFloat(float max)
{
	float maxScaledUpFloat = float(RESOLUTION) * max;
	int maxScaledUpInt = int(maxScaledUpFloat);
	int resScaledUpInt = rand() % maxScaledUpInt;
	float resScaledUpFloat = float(resScaledUpInt);
	return resScaledUpFloat / float(RESOLUTION);
}

float RandFloatGen::GetRandFloat(float min, float max)
{
	assert(max > min);
	float range = max - min;
	return min + GetRandFloat(range);
}