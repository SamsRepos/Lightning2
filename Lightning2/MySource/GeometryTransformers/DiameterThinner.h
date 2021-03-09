#pragma once

#include <vector>

#include "Segment.h"

class DiameterThinner
{
public:
	void InitParameters(
		float _scale
	);
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

private:
	std::vector<Segment*>* segments;
	float scale;
};

