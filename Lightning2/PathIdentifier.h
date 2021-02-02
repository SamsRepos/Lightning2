#pragma once

#include <vector>

#include "Segment.h"

class PathIdentifier
{
public:
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

private:
	void DescendantCounterRecurs(Segment* currentSegment);
	void StatusSetterRecursByDescendants(Segment* currentSegment);
	
	void DistFromRootSetterRecurs(Segment* root, Segment* currentSegment);
	void StatusSetterRecursByDistFromRoot(Segment* currentSegments);

	std::vector<Segment*>* segments;
};
