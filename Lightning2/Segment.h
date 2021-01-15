#pragma once

#include "DXF.h"

#include "MyVectorUtils.h"

const float MIN_SEGMENT_LENGTH   = 0.1f;
const float MAX_CHAOS_PROPORTION = 0.2f;

class Segment
{	
public:
	Segment(XMFLOAT3 startPoint, XMFLOAT3 endPoint);

	inline XMFLOAT3 GetStartPt() { return startPoint; };
	inline XMFLOAT3 GetEndPt() { return endPoint; };
	inline XMFLOAT3 GetDirection() { return endPoint - startPoint; };
	inline XMFLOAT3 GetMidpoint() { return startPoint + (GetDirection()*.5f); };
	float GetLength();

private:
	XMFLOAT3 startPoint, endPoint;
};

