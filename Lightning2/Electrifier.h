#pragma once

#include <vector>

#include "Segment.h"

class Electrifier
{	
public:
	void InitParameters(
		float maxSegLength,
		float chaosProportionToLength
	);
	inline void SetInput(std::vector<Segment>* inputVector) { input = inputVector; };
	void Run();
	inline std::vector<Segment>& GetOutput() { return *currentSegments; };

private:
	void InitAlgorithm();
	void ResetSegmentVectors();
	void SwapSegmentsVectors();
	std::vector<Segment> JitterSegment(Segment& segment, float extent);
	
	std::vector<Segment>* input;
	float maxLength = 1.f;
	float chaosProportion = .1f;
		
	std::vector<Segment> segmentsA;
	std::vector<Segment> segmentsB;
	std::vector<Segment>* currentSegments;
	std::vector<Segment>* nextSegments;
};

