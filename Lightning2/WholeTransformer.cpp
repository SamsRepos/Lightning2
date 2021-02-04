#include "WholeTransformer.h"

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

	MyFloat3 a;
	a.x
	// 3. scale to start->end magnitude
	// 4. transform to start point
}

struct MyFloat4
{

	float x;
	float y;
	float z;
	float w;

	MyFloat4()
	{
		x = y = z = 0.f;
		w = 1.f;
	}

	MyFloat4(float _x, float _y, float _z, float _w = 1.f)
	{
		x = _x;
		y = _y;
		z = _z;
		w = _w;
	}

	float Magnitude();
	
	MyFloat3 Normalised();
};

class MyMatrix44
{
	MyFloat4 values[4];
};

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

void WholeTransformer::AlignSegments(MyFloat3 v)
{

}