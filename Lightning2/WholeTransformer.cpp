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

	// 1. transform to origin
	TranslateRecurs(root, MyFloat3(0.f, 0.f, 0.f));

	// 2. align along start->end vector
	MyFloat3 desiredDirection = endPoint - startPoint;
	MyFloat3 currentDirection = GetFarthestEndPointRecurs(root);
	AlignSegments(desiredDirection, currentDirection);

	// 3. scale to start->end magnitude
	float scaleFactor = desiredDirection.Magnitude() / currentDirection.Magnitude();
	if (scaleFactor != 0.f)
	{
		ScaleSegments(scaleFactor);
	}

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

void WholeTransformer::AlignSegments(MyFloat3 desiredDirection, MyFloat3 currentDirection)
{
	MyFloat3 rotationAxis = CrossProduct(desiredDirection, currentDirection).Normalised();

	float alpha = rotationAxis.x;
	float beta  = rotationAxis.y;
	float gamma = rotationAxis.z;

	float alphaSquared = alpha * alpha;
	float betaSquared  = beta * beta;
	float gammaSquared = gamma * gamma;

	float alphaBeta  = alpha * beta;
	float alphaGamma = alpha * gamma;
	float betaGamma  = beta * gamma;

	float cosTheta = DotProduct(desiredDirection.Normalised(), currentDirection.Normalised());
	float theta    = -acos(cosTheta);
	float sinTheta = sin(theta);

	float rotationValues[] = {
		(alphaSquared * (1 - cosTheta) + cosTheta),      (alphaBeta * (1 - cosTheta) + gamma * sinTheta), (alphaGamma * (1 - cosTheta) - beta * sinTheta), 0.f,
		(alphaBeta * (1 - cosTheta) - gamma * sinTheta), (betaSquared * (1 - cosTheta) + cosTheta),       (betaGamma * (1 - cosTheta) - alpha * sinTheta), 0.f,
		(alphaGamma * (1 - cosTheta) + beta * sinTheta), (betaGamma * (1 - cosTheta) - alpha * sinTheta), (gammaSquared * (1 - cosTheta) + cosTheta),      0.f,
		0.f,                                             0.f,                                             0.f,                                             1.f
	};	

	MyMatrix44 rotationMatrix(rotationValues);

	for (Segment* segment : *segments)
	{
		segment->SetStartPoint(segment->GetStartPoint() * rotationMatrix);
		segment->SetEndPoint(segment->GetEndPoint() * rotationMatrix);
	}

}

void WholeTransformer::ScaleSegments(float factor)
{
	for (Segment* segment : *segments)
	{
		segment->SetStartPoint(
			MyFloat3(
				segment->GetStartPoint().x * factor,
				segment->GetStartPoint().y * factor,
				segment->GetStartPoint().z * factor
			)
		);
		segment->SetEndPoint(
			MyFloat3(
				segment->GetEndPoint().x * factor,
				segment->GetEndPoint().y * factor,
				segment->GetEndPoint().z * factor
			)
		);
		segment->SetDiameter(segment->GetDiameter() * factor);
	}
}