#include "Electrifier.h"

#include <algorithm>

#include "SegmentRemoval.h"
#include "DefaultParameters.h"
#include "MyClamp.h"

////
// PUBLIC:
////

void Electrifier::InitParameters(float maxSegLength, float chaosMean, float chaosStdDev)
{	
	maxLength = MyClamp(maxSegLength,
		E_MIN_MAX_SEG_LENGTH,
		E_MAX_MAX_SEG_LENGTH
	);

	chaosGaussianGen.SetMean(chaosMean);
	chaosGaussianGen.SetStdDev(chaosStdDev);
}

void Electrifier::Run()
{
	previousSegments = new std::vector<Segment*>;
	
	for (Segment* segment : *segments)
	{
		previousSegments->push_back(new Segment(*segment));
	}
	
	bool anySegmentTooLong;
	do
	{
		currentSegments = new std::vector<Segment*>;

		anySegmentTooLong = false;

		for (Segment* seg : *previousSegments)
		{
			float len = seg->GetLength();

			if (len > maxLength)
			{
				anySegmentTooLong = true;

				std::vector<Segment*> res = JitterSegment(seg, len * chaosGaussianGen.GetSample());
				currentSegments->insert(currentSegments->end(), res.begin(), res.end());
			}
			else
			{
				currentSegments->push_back(new Segment(*seg));
			}
		}

		//prep for the next iteration:
		ClearAllSegmentData(previousSegments);
		delete previousSegments;
		previousSegments = currentSegments;

	} while(anySegmentTooLong);

	//output vector:
	if (segments)
	{
		ClearAllSegmentData(segments);
		for (Segment* segment : *currentSegments)
		{
			segments->push_back(new Segment(*segment));
		}
	}

	//cleaning up working vectors:
	delete currentSegments;
	currentSegments = NULL;
	previousSegments = NULL;
}

////
// PRIVATE:
////

std::vector<Segment*> Electrifier::JitterSegment(Segment* seed, float extent)
{
	//1. get a random vector
	MyFloat3 randvec = RandomNormalisedVector();

	//2. get the normalised cross product of the current segment's dir vector, and a random vector
	MyFloat3 offset = CrossProduct(randvec, seed->GetDirection());
	offset          = offset.Normalised();
	//3. multiply that by by chaos factor
	offset = offset * extent;

	//4. get new point
	MyFloat3 newPt = seed->GetMidpoint() + offset;

	//5. two resulting segments
	Segment* topSeg    = new Segment(seed->GetStartPoint(), newPt);
	Segment* bottomSeg = new Segment(newPt, seed->GetEndPoint());

	std::vector<Segment*> res = { topSeg, bottomSeg };

	return res;
}
