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
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();
	
private:
	void InitAlgorithm();
	void ResetSegmentVectors();
	void SwapSegmentsVectors();
	std::vector<Segment*> JitterSegment(Segment* segment, float extent);
	
	std::vector<Segment*>* segments;
	float maxLength = 1.f;
	float chaosProportion = .1f;
		
	std::vector<Segment*> segmentsA;
	std::vector<Segment*> segmentsB;
	std::vector<Segment*>* currentSegments;
	std::vector<Segment*>* nextSegments;
};
