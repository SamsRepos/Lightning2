#include "BoxMullerGen.h"


BoxMullerGen::BoxMullerGen(float mean, float stdDev)
	:
	mean(mean),
	stdDev(stdDev)	
{
}

float BoxMullerGen::GetSample()
{	
	if (!hasStoredValue)
	{
		float v1, v2, rsq, fac;

		do
		{
			v1 = randFloatGen.GetRandFloat(-1.f, 1.f);
			v2 = randFloatGen.GetRandFloat(-1.f, 1.f);
			rsq = v1 * v1 + v2 * v2;
		} while (rsq == 0.f || rsq >= 1.f);
		
		fac = sqrt((-2.f * log(rsq)) / rsq);		
		storedValue = v1 * fac;
		hasStoredValue = true;
		return mean + (stdDev * v2 * fac);
	}
	else
	{
		hasStoredValue = false;
		return mean + (stdDev * storedValue);
	}
}