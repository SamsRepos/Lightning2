#pragma once

#include <vector>

#include "Segment.h"

class PathIdentifier
{
public:
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

private:
	void RunRecurs(Segment* root, Segment* currentSegment);
	//void StatusSetterRecurs(Segment* currentSegment);

	std::vector<Segment*>* segments;
};
