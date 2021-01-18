#include "Electrifier.h"

#include <algorithm>

////
// PUBLIC:
////

void Electrifier::InitParameters(float maxSegLength, float chaosProportionToLength)
{	
	chaosProportion = std::min(
		std::max(chaosProportionToLength, 0.f),
		MAX_CHAOS_PROPORTION
	);

	maxLength = std::max(maxSegLength, MIN_SEGMENT_LENGTH);	
}

void Electrifier::Run()
{
	InitAlgorithm();
	
	bool anySegmentTooLong;
	do
	{
		anySegmentTooLong = false;

		for (auto& seg : *currentSegments)
		{
			float len = seg.GetLength();

			if (len > maxLength)
			{
				anySegmentTooLong = true;

				std::vector<Segment> res = JitterSegment(seg, len * chaosProportion);
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
}

////
// PRIVATE:
////

void Electrifier::InitAlgorithm()
{
	ResetSegmentVectors();
	*currentSegments = *input;
}

void Electrifier::ResetSegmentVectors()
{
	segmentsA.clear();
	segmentsB.clear();

	currentSegments = &segmentsA;
	nextSegments = &segmentsB;
}

void Electrifier::SwapSegmentsVectors()
{
	currentSegments = (currentSegments == &segmentsA) ? &segmentsB : &segmentsA;
	nextSegments    = (nextSegments    == &segmentsA) ? &segmentsB : &segmentsA;
}

std::vector<Segment> Electrifier::JitterSegment(Segment& seed, float extent)
{
	//1. get a random vector
	MyFloat3 randvec = RandomNormalisedVector();

	//2. get the normalised cross product of the current segment's dir vector, and a random vector
	MyFloat3 offset = CrossProduct(randvec, seed.GetDirection());
	offset = offset.Normalised();
	//3. multiply that by by chaos factor
	offset = offset * extent;

	//4. get new point
	MyFloat3 newPt = seed.GetMidpoint() + offset;

	//5. two resulting segments
	Segment topSeg(seed.GetStartPoint(), newPt);
	Segment bottomSeg(newPt, seed.GetEndPoint());

	std::vector<Segment> res = { topSeg, bottomSeg };

	return res;
}
