#include "Segment.h"

#include <math.h>

Segment::Segment(MyFloat3 startPoint, MyFloat3 endPoint, float diameter, float minDiameter)
	:
	startPoint(startPoint),
	endPoint(endPoint),
	diameter(diameter),
	minDiameter(minDiameter),
	parent(NULL)
{
}

float Segment::GetLength()
{
	MyFloat3 direction = GetDirection();
	return sqrt(
		direction.x * direction.x +
		direction.y * direction.y +
		direction.z * direction.z
	);
}

void Segment::SetToBeCulled()
{
	cullMe = true;
	for (Segment* child : children)
	{
		child->SetToBeCulled();
	}
}