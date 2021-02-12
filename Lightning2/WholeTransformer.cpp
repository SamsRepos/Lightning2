#include "WholeTransformer.h"

#include "MyMatrix44.h"

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
	// PLAN:
	// 1. transform to origin	
	// 2. align along start->end vector	
	// 3. scale to start->end magnitude
	// 4. transform to start point

	Segment* root = segments->front();

	MyFloat3 desiredDirection = endPoint - startPoint;
	MyFloat3 currentDirection = GetFarthestEndPointRecurs(root) - root->GetStartPoint();

	// 1. transform to origin
	TranslateRecurs(root, MyFloat3(0.f, 0.f, 0.f));

	// 2. align along start->end vector	
	AlignSegments(desiredDirection, currentDirection);

	// updating current direction, which fixes scaling:
	// TODO: find out why. This shouldn't be necessary. Something must be wrong with rotation.
	currentDirection = GetFarthestEndPointRecurs(root) - root->GetStartPoint();

	// 3. scale to start->end magnitude
	ScaleSegments(desiredDirection.Magnitude(), currentDirection.Magnitude());

	// 4. transform to start point
	TranslateRecurs(root, startPoint);
}

void WholeTransformer::TranslateRecurs(Segment* currentSegment, MyFloat3 currentStartPoint)
{
	MyFloat3 direction = currentSegment->GetDirection();
	currentSegment->SetStartPoint(currentStartPoint);
	currentSegment->SetEndPoint(currentStartPoint + direction);

	for (Segment* child : *(currentSegment->GetChildren()))
	{
		TranslateRecurs(child, currentSegment->GetEndPoint());
	}
}

MyFloat3 WholeTransformer::GetFarthestEndPointRecurs(Segment* currentSegment)
{
	if (currentSegment->GetChildren()->size() == 0)
	{
		return currentSegment->GetEndPoint();
	}
	else
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			if (child->GetStatus() == SegmentStatuses::PRIMARY)
			{
				return GetFarthestEndPointRecurs(child);
			}
		}
	}
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