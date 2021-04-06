#include "Segment.h"

#include <math.h>

Segment::Segment(MyFloat3 startPoint, MyFloat3 endPoint, float diameter, float minDiameter)
	:
	startPoint(startPoint),
	endPoint(endPoint),
	diameter(diameter),
	minDiameter(minDiameter),
	parent(NULL),
	cullMe(false),
	energy(1.f),
	velocity(1.f)
{
}

Segment::~Segment()
{

}

void Segment::SetToBeCulled()
{
	cullMe = true;
	for (Segment* child : children)
	{
		child->SetToBeCulled();
	}
}