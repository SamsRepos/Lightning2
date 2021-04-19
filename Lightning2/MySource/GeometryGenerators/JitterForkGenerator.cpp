#include "JitterForkGenerator.h"

#include <algorithm>

#include "Utils/MyVectorUtil.h"
#include "Maths/MyClamp.h"
#include "DefaultParameters.h"

////
// PUBLIC:
////

JitterForkGenerator::JitterForkGenerator()
	:
	output(NULL)
{
	chaosGaussianGen.SetMean(DEFAULT_JFG_CHAOS_MEAN);
	chaosGaussianGen.SetStdDev(DEFAULT_JFG_CHAOS_STDDEV);
	midpointGaussianGen.SetMean(.5f);
	midpointGaussianGen.SetStdDev(DEFAULT_JFG_MIDPOINT_STDDEV);
}

void JitterForkGenerator::InitParameters(
	const Segment& seedSegment,
	size_t its,
	float chaosProportionMean,
	float chaosProportionStdDev,
	float midpointStdDev,
	float baselineForkProbability,
	float forkProbabilityScaleDown
)
{
	originalSeed      = seedSegment;
	iterations        = its;
	
	chaosGaussianGen.SetMean(chaosProportionMean);
	chaosGaussianGen.SetStdDev(chaosProportionStdDev);
	midpointGaussianGen.SetStdDev(midpointStdDev);

	baselineForkProb  = baselineForkProbability;
	forkProbScaleDown = forkProbabilityScaleDown;
}

void JitterForkGenerator::Run()
{
	recursCapHit = false;

	previousSegments = new std::vector<Segment*>;
	previousSegments->push_back(new Segment(originalSeed));

	float forkProb = baselineForkProb;

	for (size_t i = 0; i < iterations; i++)
	{
		currentSegments = new std::vector<Segment*>;
		
		Segment* root = previousSegments->front();
		
		RunIterationRecurs(root, forkProb, NULL, 0);
		
		//Prep for the next iteration:
		DeleteAllVectorData(previousSegments);
		delete previousSegments;
		previousSegments = currentSegments;
		
		forkProb *= forkProbScaleDown;

		if (recursCapHit)
		{
			break;
		}
	}

	//setting output:
	output = new std::vector<Segment*>;
	for (Segment* segPtr : *currentSegments)
	{
		output->push_back(segPtr);
	}

	//cleaning up the remaining working vector:
	//note: not calling DeleteAllVectorData(currentSegments) because the dynamically allocated segments are now in output
	//note: currentSegments and previousSegments now point to the same vector
	delete currentSegments;
	currentSegments  = NULL;
	previousSegments = NULL; 

	//Note, for all segments, energy and velocity are left as default - not provided by this method	
}

////
// PRIVATE:
////

void JitterForkGenerator::RunIterationRecurs(Segment* seed, float forkProb, Segment* parentSegment, size_t recursCount)
{
	std::vector<Segment*> res = JitterAndFork(seed, forkProb, parentSegment);
	currentSegments->insert(currentSegments->end(), res.begin(), res.end());

	Segment* nextParent = res[1]; //bottomSeg

	if (recursCount < RECURSIVE_CAP)
	{
		for (Segment* seedChild : *(seed->GetChildren()))
		{
			RunIterationRecurs(seedChild, forkProb, nextParent, recursCount+1);
		}
	}
	else
	{
		recursCapHit = true;
	}
}

std::vector<Segment*> JitterForkGenerator::JitterAndFork(Segment* seed, float forkProbNow, Segment* parent)
{
	// 1. Get offset point
	MyFloat3 offset       = RandomPerpendicularUnitVector(seed->GetDirection());
	float chaosProportion = chaosGaussianGen.GetSample();
	float chaos           = seed->GetLength() * chaosProportion;
	offset                = offset * chaos;
	MyFloat3 offsetPoint  = seed->GetOffsetMidpoint(midpointGaussianGen.GetSample()) + offset;

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

		res.push_back(forkedSeg);
	}
		
	return res;
}
