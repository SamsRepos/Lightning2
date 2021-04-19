#include "WholeTransformer.h"

#include "Maths/MyMatrix44.h"
#include "DefaultParameters.h"

void WholeTransformer::InitParameters(
	MyFloat3 _startPoint,
	MyFloat3 _endPoint
)
{
	startPoint = _startPoint;
	endPoint   = _endPoint;
}

void WholeTransformer::Run()
{
	recursCapHit = false;

	// PLAN:
	// 1. transform to origin	
	// 2. align along start->end vector	
	// 3. scale to start->end magnitude
	// 4. transform to start point

	Segment* root = segments->front();

	MyFloat3 desiredDirection = endPoint - startPoint;
	MyFloat3 currentDirection = GetFarthestEndPointRecurs(root, 0) - root->GetStartPoint();

	// 1. transform to origin
	TranslateRecurs(root, MyFloat3(0.f, 0.f, 0.f), 0);

	// 2. align along start->end vector	
	AlignSegments(desiredDirection, currentDirection);
	
	// 3. scale to start->end magnitude
	currentDirection = GetFarthestEndPointRecurs(root, 0) - root->GetStartPoint();
	ScaleSegments(desiredDirection.Magnitude(), currentDirection.Magnitude());

	// 4. transform to start point
	TranslateRecurs(root, startPoint, 0);
}

void WholeTransformer::TranslateRecurs(Segment* currentSegment, MyFloat3 currentStartPoint, size_t recursCount)
{
	MyFloat3 direction = currentSegment->GetDirection();
	currentSegment->SetStartPoint(currentStartPoint);
	currentSegment->SetEndPoint(currentStartPoint + direction);

	if (recursCount < RECURSIVE_CAP)
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			TranslateRecurs(child, currentSegment->GetEndPoint(), recursCount+1);
		}
	}
	else
	{
		recursCapHit = true;
	}
}

MyFloat3 WholeTransformer::GetFarthestEndPointRecurs(Segment* currentSegment, size_t recursCount)
{
	if (recursCount < RECURSIVE_CAP)
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			if (child->GetStatus() == SegmentStatuses::PRIMARY)
			{
				return GetFarthestEndPointRecurs(child, recursCount+1);
			}
		}
	}
	else
	{
		recursCapHit = true;
	}
	
	return currentSegment->GetEndPoint();	
}

void WholeTransformer::AlignSegments(const MyFloat3& desiredDirection, const MyFloat3& currentDirection)
{
	MyFloat3 rotationAxis = CrossProduct(desiredDirection, currentDirection).Normalised();
		
	float cosAngle = DotProduct(desiredDirection.Normalised(), currentDirection.Normalised());
	float angle    = acos(cosAngle);

	MyMatrix44 rotationMatrix = RotationMatrix(rotationAxis, angle);

	for (Segment* segment : *segments)
	{
		MyFloat3 newStartPoint = segment->GetStartPoint() * rotationMatrix;
		segment->SetStartPoint(newStartPoint);

		MyFloat3 newEndPoint = segment->GetEndPoint() * rotationMatrix;
		segment->SetEndPoint(newEndPoint);
	}
}

void WholeTransformer::ScaleSegments(const float& desiredMagnitude, const float& currentMagnitude)
{
	float factor;
	if (currentMagnitude != 0.f)
	{
		factor = desiredMagnitude / currentMagnitude;
	}
	else
	{
		return;
	}

	for (Segment* segment : *segments)
	{
		segment->SetStartPoint(segment->GetStartPoint() * factor);
		segment->SetEndPoint(segment->GetEndPoint() * factor);
		segment->SetDiameter(segment->GetDiameter() * factor);
	}
}