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
	previousSegments = new std::vector<Segment*>;
	previousSegments->push_back(new Segment(originalSeed));

	float forkProb = baselineForkProb;

	for (size_t i = 0; i < iterations; i++)
	{
		currentSegments = new std::vector<Segment*>;

		Segment* root = previousSegments->front();

		TODO sort this out!

		for (Segment* seg : *previousSegments)
		{
			Segment* previousParent = seg->GetParent();
			std::vector<Segment*>* previousChildren = seg->GetChildren();

			std::vector<Segment*> res = JitterAndFork(seg, forkProb, previousParent, previousChildren);
			currentSegments->insert(currentSegments->end(), res.begin(), res.end());
		}

		//Prep for the next iteration:
		ClearAllSegmentData(previousSegments);
		delete previousSegments;
		previousSegments = currentSegments;
		
		forkProb *= forkProbScaleDown;
	}

	//setting output:
	output = new std::vector<Segment*>;
	for (Segment* seg : *currentSegments)
	{
		output->push_back(new Segment(*seg));
	}

	//cleaning up the remaining working vector:
	ClearAllSegmentData(currentSegments);
	//note: current and previous segments now point to the same vector
	delete currentSegments;
	currentSegments  = NULL;
	previousSegments = NULL; 
}

////
// PRIVATE:
////

std::vector<Segment*> JitterForkGenerator::JitterAndFork(
	Segment* seed,
	float forkProbNow,
	Segment* previousParent,
	std::vector<Segment*>* previousChildren
)
{
	// 1. Get offset point
	MyFloat3 offset = RandomPerpendicularUnitVector(seed->GetDirection());
	float chaos = seed->GetLength() * chaosProportion;
	offset = offset * chaos;
	MyFloat3 offsetPoint = seed->GetMidpoint() + offset;

	// 2. Generate two new segments
	Segment* topSeg    = new Segment(seed->GetStartPoint(), offsetPoint);
	Segment* bottomSeg = new Segment(offsetPoint, seed->GetEndPoint());

	// 2.2 parenting:
	topSeg->SetParent(previousParent);
	topSeg->AddChild(bottomSeg);
	bottomSeg->SetParent(topSeg);
	for (Segment* previousChild : *previousChildren)
	{
		bottomSeg->AddChild(previousChild);
	}

	std::vector<Segment*> res = { topSeg, bottomSeg };

	// 3. Maybe fork
	if(randFloatGen.GetRandFloat(1.f) < forkProbNow)
	{
		Segment* forkedSeg = new Segment(
			offsetPoint,
			offsetPoint + topSeg->GetDirection()
		);

		forkedSeg->SetParent(topSeg);
		topSeg->AddChild(forkedSeg);

		res.push_back(
			forkedSeg
		);
	}
		
	return res;
}
