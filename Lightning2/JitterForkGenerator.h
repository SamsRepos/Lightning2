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
	void InitAlgorithm();
	void ResetSegmentVectors();
	void SwapSegmentsVectors();
	std::vector<Segment*> JitterAndFork(Segment* segment, float forkProbNow);

	Segment originalSeed;
	int iterations;
	float chaosProportion;
	float baselineForkProb;
	float forkProbScaleDown;
	
	std::vector<Segment*> segmentsA;
	std::vector<Segment*> segmentsB;
	std::vector<Segment*>* currentSegments;
	std::vector<Segment*>* nextSegments;

	std::vector<Segment*>* output;

	RandFloatGen randFloatGen;
};
