#pragma once

#include <vector>

#include "Segment.h"
#include "BoxMullerGen.h"

class Electrifier
{	
public:
	void InitParameters(
		float maxSegLength,
		float chaosMean,
		float chaosStdDev
	);
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();
	
private:
	std::vector<Segment*> JitterSegment(Segment* segment, float extent);
	
	std::vector<Segment*>* segments;

	float maxLength;

	std::vector<Segment*>* previousSegments;
	std::vector<Segment*>* currentSegments;

	BoxMullerGen chaosGaussianGen;
};
