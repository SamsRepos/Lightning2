#pragma once

#include "MyFloat3.h"

const float MIN_SEGMENT_LENGTH   = 0.1f;
const float MAX_CHAOS_PROPORTION = 0.2f;

class Segment
{	
public:
	Segment(MyFloat3 startPoint, MyFloat3 endPoint);

	inline MyFloat3 GetStartPoint() { return startPoint; };
	inline MyFloat3 GetEndPoint() { return endPoint; };
	inline MyFloat3 GetDirection() { return endPoint - startPoint; };
	inline MyFloat3 GetMidpoint() { return startPoint + (GetDirection()*.5f); };
	float GetLength();

private:
	MyFloat3 startPoint, endPoint;
};

