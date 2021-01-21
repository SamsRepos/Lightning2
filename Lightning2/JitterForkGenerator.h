#pragma once

#include <vector>

#include "Segment.h"
#include "RandFloatGen.h"

class JitterForkGenerator
{	
public:
	JitterForkGenerator();

	void InitParameters(
		const Segment& seedSegment,
		size_t its,
		float chaosProportionToLength,
		float forkProbability,
		float forkProbabilityScaleDown
	);
	
	void Run();
	inline std::vector<Segment*>* GetOutput() { return output; };

private:
	std::vector<Segment*> JitterAndFork(
		Segment* segment,
		float forkProbNow,
		Segment* previousParent,
		std::vector<Segment*>* previousChildren
	);

	Segment originalSeed;
	int iterations;
	float chaosProportion;
	float baselineForkProb;
	float forkProbScaleDown;
	
	std::vector<Segment*>* previousSegments;
	std::vector<Segment*>* currentSegments;

	std::vector<Segment*>* output;

	RandFloatGen randFloatGen;
};
