#pragma once

#include <vector>

#include "Segment.h"
#include "Maths/BoxMullerGen.h"

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
	
	inline bool WasRecursCapHit() { return recursCapHit; };

private:
	bool RunIterationRecurs(Segment* seed, Segment* parentSegment, size_t recursCount);
	std::vector<Segment*> Jitter(Segment* segment, Segment* parent);
	
	std::vector<Segment*>* segments;

	float maxLength;

	std::vector<Segment*>* previousSegments;
	std::vector<Segment*>* currentSegments;

	BoxMullerGen chaosGaussianGen;

	bool recursCapHit;
};
