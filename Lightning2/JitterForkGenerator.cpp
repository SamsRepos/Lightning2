#include "JitterForkGenerator.h"

#include <algorithm>

#include "SegmentRemoval.h"
#include "MyClamp.h"

////
// PUBLIC:
////

JitterForkGenerator::JitterForkGenerator()
	:
	output(NULL)
{
}

void JitterForkGenerator::InitParameters(
	const Segment& seedSegment,
	size_t its,
	float chaosProportionToLength,
	float baselineForkProbability ,
	float forkProbabilityScaleDown
)
{
	originalSeed      = seedSegment;
	iterations        = its;
	chaosProportion   = MyClamp(chaosProportionToLength, 0.f, 1.f);
	baselineForkProb  = baselineForkProbability;
	forkProbScaleDown = forkProbabilityScaleDown;
}

void JitterForkGenerator::Run()
{
	InitAlgorithm();

	float forkProb = baselineForkProb;

	for (size_t i = 0; i < iterations; i++)
	{
		for (Segment* seg : *currentSegments)
		{
			std::vector<Segment*> res = JitterAndFork(seg, forkProb);
			nextSegments->insert(nextSegments->end(), res.begin(), res.end());
		}

		SwapSegmentsVectors();
		nextSegments->clear();

		forkProb *= forkProbScaleDown;
	}

	output = new std::vector<Segment*>;
	for (Segment* seg : *currentSegments)
	{
		output->push_back(new Segment(*seg));
	}
}

////
// PRIVATE:
////

void JitterForkGenerator::InitAlgorithm()
{
	ResetSegmentVectors();
	currentSegments->push_back(new Segment(originalSeed));
}

void JitterForkGenerator::ResetSegmentVectors()
{
	ClearAllSegmentData(&segmentsA);
	ClearAllSegmentData(&segmentsB);

	currentSegments = &segmentsA;
	nextSegments    = &segmentsB;
}

void JitterForkGenerator::SwapSegmentsVectors()
{
	currentSegments = (currentSegments == &segmentsA) ? &segmentsB : &segmentsA;
	nextSegments    = (nextSegments    == &segmentsA) ? &segmentsB : &segmentsA;
}

std::vector<Segment*> JitterForkGenerator::JitterAndFork(Segment* seed, float forkProbNow)
{
	// 1. Get offset point
	MyFloat3 offset = RandomPerpendicularUnitVector(seed->GetDirection());
	float chaos = seed->GetLength() * chaosProportion;
	offset = offset * chaos;
	MyFloat3 offsetPoint = seed->GetMidpoint() + offset;

	// 2. Generate two new segments
	Segment* topSeg    = new Segment(seed->GetStartPoint(), offsetPoint);
	Segment* bottomSeg = new Segment(offsetPoint, seed->GetEndPoint());

	std::vector<Segment*> res = { topSeg, bottomSeg };

	// 3. Maybe fork
	if(randFloatGen.GetRandFloat(1.f) < forkProbNow)
	{
		res.push_back(
			new Segment(
				offsetPoint,
				offsetPoint + topSeg->GetDirection()
			)
		);
	}

	return res;
}
