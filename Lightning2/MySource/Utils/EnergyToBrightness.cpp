#include "EnergyToBrightness.h"

#include "Maths/MyClamp.h"

float MaxEnergy(std::vector<AnimSegment*>* animSegs)
{
	float maxEnergy = 0.f;
	for (AnimSegment* animSeg : *animSegs)
	{
		if (animSeg->GetEnergy() > maxEnergy)
		{
			maxEnergy = animSeg->GetEnergy();
		}
	}
	return maxEnergy;
}

float EnergyToBrightness(float energy, float maxEnergy, EnergyScales energyScale)
{
	float res;

	switch (energyScale)
	{
	case(EnergyScales::LINEAR):
		res = (energy / maxEnergy);
		break;
	case(EnergyScales::LN):
		res = log(energy) / log(maxEnergy);
		break;
	case(EnergyScales::LOG_10):
		res = log10(energy) / log10(maxEnergy);
		break;
	default:
		res = 1.f;
		break;
	}

	return MyClamp(res, 0.f, 1.f);
}