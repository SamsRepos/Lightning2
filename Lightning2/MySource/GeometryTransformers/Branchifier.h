#pragma once

#include <vector>

#include "Segment.h"

class Branchifier
{
public:
	void InitParameters(
		float _initialDiameter,
		float _diameterScaledown,
		size_t _maxNumBranchLevels
	);
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

private:
	std::vector<Segment*>* segments;

	float initialDiameter;
	float diamterScaledown;
	size_t maxNumBranchLevels;
	
	void DiameterTransformRecurs(Segment* segment, float diameter, size_t branchLevel);
};
