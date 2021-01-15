#include "Electrifier.h"

////
// PUBLIC:
////

void Electrifier::InitParameters(float maxSegLength, float chaosProportionToLength)
{	
	chaosProportion = min(
		max(chaosProportionToLength, 0.f),
		MAX_CHAOS_PROPORTION
	);

	maxLength = max(maxSegLength, MIN_SEGMENT_LENGTH);	
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
	currentSegments = currentSegments == &segmentsA ? &segmentsB : &segmentsA;
	nextSegments    = nextSegments    == &segmentsA ? &segmentsB : &segmentsA;
}

std::vector<Segment> Electrifier::JitterSegment(Segment& seed, float extent)
{
	//1. get a random vector
	XMFLOAT3 randvec = randomNormalisedVector();

	//2. get the normalised cross product of the current segment's dir vector, and a random vector
	XMFLOAT3 offset = crossProduct(randvec, seed.GetDirection());
	offset = normalised(offset);
	//3. multiply that by by chaos factor
	offset = offset * extent;

	//4. get new point
	XMFLOAT3 newPt = seed.GetMidpoint() + offset;

	//5. two resulting segments
	Segment topSeg(seed.GetStartPt(), newPt);
	Segment bottomSeg(newPt, seed.GetEndPt());

	std::vector<Segment> res = { topSeg, bottomSeg };

	return res;
}
