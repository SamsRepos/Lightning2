#include "Segment.h"



Segment::Segment(XMFLOAT3 startPoint, XMFLOAT3 endPoint)
	:
	startPoint(startPoint),
	endPoint(endPoint)
{
}

float Segment::GetLength()
{ 
	XMFLOAT3 direction = GetDirection();
	return sqrt(
		direction.x * direction.x +
		direction.y * direction.y +
		direction.z * direction.z
	);
}
