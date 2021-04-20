#pragma once

#include "Graphics/RenderSettings.h"
#include "Graphics/AnimSegment.h"

float MaxEnergy(std::vector<AnimSegment*>* animSegs);

float EnergyToBrightness(float energy, float maxEnergy, EnergyScales energyScale);