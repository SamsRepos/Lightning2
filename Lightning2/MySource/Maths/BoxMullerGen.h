#pragma once

#include <cmath>

#include "Maths/RandFloatGen.h"

class BoxMullerGen
{
public:
	BoxMullerGen(float mean = 0.f, float stdDev = 1.f);
	float GetSample();

	inline void SetMean(float m) { mean = m; };
	inline void SetStdDev(float sd) { stdDev = sd; };
private:
	float mean;
	float stdDev;
	
	RandFloatGen randFloatGen;
	bool hasStoredValue = false;
	float storedValue;
};

