#pragma once

#include <vector>

#include "Segment.h"

class PathIdentifier
{
public:
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

	inline bool WasRecursCapHit() { return recursCapHit; };

private:
	void RunRecurs(Segment* root, Segment* currentSegment, float distFromRoot, size_t recursCount);
	//void StatusSetterRecurs(Segment* currentSegment);

	std::vector<Segment*>* segments;

	bool recursCapHit;
};
