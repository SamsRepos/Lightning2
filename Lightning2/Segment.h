#pragma once

#include <vector>

#include "MyFloat3.h"

const float MIN_SEGMENT_LENGTH   = 0.1f;
const float MAX_CHAOS_PROPORTION = 0.2f;

enum SegmentStatuses
{
	PRIMARY,
	SECONDARY
};

class Segment
{	
public:

	Segment(
		MyFloat3 startPoint = MyFloat3(0.f, 0.f, 0.f),
		MyFloat3 endPoint   = MyFloat3(0.f, 0.f, 0.f),
		float diameter      = 1.f,
		float minDiameter   = 1.f
	);

	inline MyFloat3 GetStartPoint() { return startPoint; };
	inline MyFloat3 GetEndPoint() { return endPoint; };
	inline MyFloat3 GetDirection() { return endPoint - startPoint; };
	inline MyFloat3 GetMidpoint() { return startPoint + (GetDirection()*.5f); };
	inline float GetDiameter() const { return diameter; };
	inline float GetMinDiameter() const { return minDiameter; };
	float GetLength();

	inline void SetParent(Segment* _parent) { parent = _parent; };
	inline void AddChild(Segment* child) { children.push_back(child); };
	
private:
	MyFloat3 startPoint, endPoint;
	float diameter, minDiameter;

	Segment* parent = NULL;
	std::vector<Segment*> children;

	int numDescendants;
	SegmentStatuses status;
};

