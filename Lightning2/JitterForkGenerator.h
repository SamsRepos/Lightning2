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
	inline std::vector<Segment*>* GetOutput() { return currentSegments; };	

private:
	void InitAlgorithm();
	void ResetSegmentVectors();
	void SwapSegmentsVectors();
	std::vector<Segment> JitterAndFork(Segment& segment, float forkProbNow);

	Segment originalSeed;
	int iterations = 10;		
	float chaosProportion = .1f;	
	float forkProb = .7f;		
	float forkProbScaleDown = .6f;
	//float forkLength = 1.f;

	std::vector<Segment*> segmentsA;
	std::vector<Segment*> segmentsB;
	std::vector<Segment*>* currentSegments;
	std::vector<Segment*>* nextSegments;

	RandFloatGen randFloatGen;
};

