#include "JitterForkGenerator.h"

#include <algorithm>

#include "MyClamp.h"

////
// PUBLIC:
////

JitterForkGenerator::JitterForkGenerator()
{
}

void JitterForkGenerator::InitParameters(
	const Segment& seedSegment,
	size_t its,
	float chaosProportionToLength,
	float forkProbability,
	float forkProbabilityScaleDown
)
{
	originalSeed      = seedSegment;
	iterations        = its;
	chaosProportion   = MyClamp(chaosProportionToLength, 0.f, 1.f);
	forkProb          = forkProbability;
	forkProbScaleDown = forkProbabilityScaleDown;
}

void JitterForkGenerator::Run()
{
	InitAlgorithm();

	float forkProbNow = forkProb;

	for (size_t i = 0; i < iterations; i++)
	{
		for (auto& seg : *currentSegments)
		{
			std::vector<Segment> res = JitterAndFork(seg, forkProbNow);
			nextSegments->insert(nextSegments->end(), res.begin(), res.end());
		}

		SwapSegmentsVectors();
		nextSegments->clear();

		forkProbNow *= forkProbScaleDown;
	}
}

////
// PRIVATE:
////

void JitterForkGenerator::InitAlgorithm()
{
	ResetSegmentVectors();
	currentSegments->push_back(originalSeed);
}

void JitterForkGenerator::ResetSegmentVectors()
{
	segmentsA.clear();
	segmentsB.clear();

	currentSegments = &segmentsA;
	nextSegments = &segmentsB;
}

void JitterForkGenerator::SwapSegmentsVectors()
{
	currentSegments = (currentSegments == &segmentsA) ? &segmentsB : &segmentsA;
	nextSegments    = (nextSegments    == &segmentsA) ? &segmentsB : &segmentsA;
}

std::vector<Segment> JitterForkGenerator::JitterAndFork(Segment& seed, float forkProbNow)
{
	// 1. Get offset point
	MyFloat3 offset = RandomPerpendicularUnitVector(seed.GetDirection());
	float chaos = seed.GetLength() * chaosProportion;
	offset = offset * chaos;
	MyFloat3 offsetPoint = seed.GetMidpoint() + offset;

	// 2. Generate two new segments
	Segment topSeg(seed.GetStartPoint(), offsetPoint);
	Segment bottomSeg(offsetPoint, seed.GetEndPoint());

	std::vector<Segment> res = { topSeg, bottomSeg };

	// 3. Maybe fork
	if(randFloatGen.GetRandFloat(1.f) < forkProbNow)
	{
		res.push_back(
			Segment(
				offsetPoint,
				offsetPoint + topSeg.GetDirection()
			)
		);
	}

	return res;
}
