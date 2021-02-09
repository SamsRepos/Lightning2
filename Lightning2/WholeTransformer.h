#pragma once

#include "MyFloat3.h"
#include "Segment.h"

class WholeTransformer
{
public:
	void InitParameters(
		MyFloat3 _startPoint,
		MyFloat3 _endPoint
	);
	inline void SetSegments(std::vector<Segment*>* _segments) { segments = _segments; };
	void Run();

private:
	void TranslateRecurs(Segment* currentSegment, MyFloat3 currentStartPoint);
	MyFloat3 GetFarthestEndPointRecurs(Segment* currentSegment);
	void AlignSegments(MyFloat3 desiredDirection, MyFloat3 currentDirection);
	void ScaleSegments(float factor);

	std::vector<Segment*>* segments;

	MyFloat3 startPoint;
	MyFloat3 endPoint;
};
