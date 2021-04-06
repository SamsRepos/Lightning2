#pragma once

#include <vector>

#include "Segment.h"
#include "Maths/RandFloatGen.h"
#include "Maths/BoxMullerGen.h"

class JitterForkGenerator
{	
public:
	JitterForkGenerator();

	void InitParameters(
		const Segment& seedSegment,
		size_t its,
		float chaosProportionMean,
		float chaosProportionStdDev,
		float midpointStdDev,
		float baselineForkProbability,
		float forkProbabilityScaleDown
	);
	
	void Run();
	inline std::vector<Segment*>* GetOutput() { return output; };

private:

	void RunIterationRecursive(Segment* seed, float forkProb, Segment* parentSegment);

	std::vector<Segment*> JitterAndFork(Segment* seed, float forkProbNow, Segment* parent);

	Segment originalSeed;
	int iterations;
	float baselineForkProb;
	float forkProbScaleDown;
	
	std::vector<Segment*>* previousSegments;
	std::vector<Segment*>* currentSegments;

	std::vector<Segment*>* output;

	RandFloatGen randFloatGen;

	BoxMullerGen chaosGaussianGen;
	BoxMullerGen midpointGaussianGen;
};
