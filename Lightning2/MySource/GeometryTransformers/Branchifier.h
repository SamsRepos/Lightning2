#pragma once

#include <vector>

#include "Segment.h"

// The Branchifier does 3 things to make the geometry look more like cloud-to-ground lightning:
//
// 1. Transforms diameters
// 2. Sets velocities
// 3. Culls outer branches
//

class Branchifier
{
public:
	void InitParameters(
		float _initialDiameter,
		float _diameterScaledown,
		float _animationTime,
		size_t _maxNumBranchLevels
	);
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

private:
	void BranchifyRecurs(Segment* segment, float diameter, float velocity, size_t branchLevel);
	
	std::vector<Segment*>* segments;

	float initialDiameter;
	float diameterScaledown;
	float animationTime;
	size_t maxNumBranchLevels;	
};
