#include "Electrifier.h"

#include <algorithm>

#include "SegmentRemoval.h"
#include "DefaultParameters.h"
#include "MyClamp.h"

////
// PUBLIC:
////

void Electrifier::InitParameters(float maxSegLength, float chaosProportionToLength)
{	
	chaosProportion = MyClamp(
		chaosProportionToLength,
		E_MIN_CHAOS_PROPORTION,
		E_MAX_CHAOS_PROPORTION
	);

	maxLength = MyClamp(maxSegLength,
		E_MIN_MAX_SEG_LENGTH,
		E_MAX_MAX_SEG_LENGTH
	);	
}

void Electrifier::Run()
{
	InitAlgorithm();
	
	bool anySegmentTooLong;
	do
	{
		anySegmentTooLong = false;

		for (Segment* seg : *currentSegments)
		{
			float len = seg->GetLength();

			if (len > maxLength)
			{
				anySegmentTooLong = true;

				std::vector<Segment*> res = JitterSegment(seg, len * chaosProportion);
				nextSegments->insert(nextSegments->end(), res.begin(), res.end());
			}
			else
			{
				nextSegments->push_back(seg);
			}
		}

		SwapSegmentsVectors();
		nextSegments->clear();

	} while(anySegmentTooLong);

	//output vector:
	if (segments)
	{
		ClearAllSegmentData(segments);
		delete segments;
	}
	segments = new std::vector<Segment*>(*currentSegments);
}

////
// PRIVATE:
////

void Electrifier::InitAlgorithm()
{
	ResetSegmentVectors();
	*currentSegments = *segments;
}

void Electrifier::ResetSegmentVectors()
{
	ClearAllSegmentData(&segmentsA);
	ClearAllSegmentData(&segmentsB);

	currentSegments = &segmentsA;
	nextSegments    = &segmentsB;
}

void Electrifier::SwapSegmentsVectors()
{
	currentSegments = (currentSegments == &segmentsA) ? &segmentsB : &segmentsA;
	nextSegments    = (nextSegments    == &segmentsA) ? &segmentsB : &segmentsA;
}

std::vector<Segment*> Electrifier::JitterSegment(Segment* seed, float extent)
{
	//1. get a random vector
	MyFloat3 randvec = RandomNormalisedVector();

	//2. get the normalised cross product of the current segment's dir vector, and a random vector
	MyFloat3 offset = CrossProduct(randvec, seed->GetDirection());
	offset = offset.Normalised();
	//3. multiply that by by chaos factor
	offset = offset * extent;

	//4. get new point
	MyFloat3 newPt = seed->GetMidpoint() + offset;

	//5. two resulting segments
	Segment* topSeg =    new Segment(seed->GetStartPoint(), newPt);
	Segment* bottomSeg = new Segment(newPt, seed->GetEndPoint());

	std::vector<Segment*> res = { topSeg, bottomSeg };

	return res;
}
