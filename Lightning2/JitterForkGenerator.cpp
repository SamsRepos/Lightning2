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
	originalSeed = seedSegment;
	InitParameters(its, chaosProportionToLength, forkProbability, forkProbabilityScaleDown);
}

void JitterForkGenerator::InitParameters(
	size_t its,
	float chaosProportionToLength,
	float forkProbability,
	float forkProbabilityScaleDown
)
{
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
	//1. get a random vector
	MyFloat3 randvec = RandomNormalisedVector();

	//2. get the normalised cross product: segment direction X random vector
	MyFloat3 offset = CrossProduct(randvec, seed.GetDirection());
	offset = Normalised(offset);

	//3. multiply that by by chaos factor
	float chaos = seed.GetLength() * chaosProportion;
	offset = offset * chaos;

	//4. get new point
	MyFloat3 offsetPoint = seed.GetMidpoint() + offset;

	//5. two resulting segments
	Segment topSeg(seed.GetStartPoint(), offsetPoint);
	Segment bottomSeg(offsetPoint, seed.GetEndPoint());

	std::vector<Segment> res = { topSeg, bottomSeg };

	//6. fork maybe
	if (randFloatGen.GetRandFloat(1.f) < forkProbNow)
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
