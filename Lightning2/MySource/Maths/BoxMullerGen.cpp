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
		float u, v, rSquared, fac;

		do
		{
			u = randFloatGen.GetRandFloat(-1.f, 1.f);
			v = randFloatGen.GetRandFloat(-1.f, 1.f);
			rSquared = (u * u) + (v * v);
		} while (rSquared == 0.f || rSquared >= 1.f);
		
		fac = sqrt((-2.f * log(rSquared)) / rSquared);		
		storedValue = u * fac;
		hasStoredValue = true;
		return mean + (stdDev * (v * fac));
	}
	else
	{
		hasStoredValue = false;
		return mean + (stdDev * storedValue);
	}
}
