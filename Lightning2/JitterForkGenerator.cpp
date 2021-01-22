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

		still need to fix this. maybe make recursive

		Segment* root = previousSegments->front();
		
		Segment* parentSeg = NULL;
		for (Segment* seg : *previousSegments)
		{
			std::vector<Segment*> res = JitterAndFork(seg, forkProb, parentSeg);
			currentSegments->insert(currentSegments->end(), res.begin(), res.end());

			//for next itearation:
			parentSeg = res[1]; //res[1] should be "bottomSeg" from in JitterAndFork()
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
	Segment* parent
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
	if (parent)
	{
		topSeg->SetParent(parent);
		parent->AddChild(topSeg);
	}
	bottomSeg->SetParent(topSeg);
	topSeg->AddChild(bottomSeg);
	
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
