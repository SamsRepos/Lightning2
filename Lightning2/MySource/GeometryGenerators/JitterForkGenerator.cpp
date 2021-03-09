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
	previousSegments = new std::vector<Segment*>;
	previousSegments->push_back(new Segment(originalSeed));

	float forkProb = baselineForkProb;

	for (size_t i = 0; i < iterations; i++)
	{
		currentSegments = new std::vector<Segment*>;
		
		Segment* root = previousSegments->front();
		
		RunIterationRecursive(root, forkProb, NULL);
		
		//Prep for the next iteration:
		DeleteAllVectorData(previousSegments);
		delete previousSegments;
		previousSegments = currentSegments;
		
		forkProb *= forkProbScaleDown;
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

	//Energy - not provided by this method
	for (Segment* seg : *output)
	{
		seg->SetEnergy(1.f);
		seg->SetVelocity(1.f);
	}
}

////
// PRIVATE:
////

void JitterForkGenerator::RunIterationRecursive(Segment* seed, float forkProb, Segment* parentSegment)
{
	std::vector<Segment*> res = JitterAndFork(seed, forkProb, parentSegment);
	currentSegments->insert(currentSegments->end(), res.begin(), res.end());

	Segment* nextParent = res[1]; //bottomSeg

	for (Segment* seedChild : *(seed->GetChildren()))
	{
		RunIterationRecursive(seedChild, forkProb, nextParent);
	}
}

std::vector<Segment*> JitterForkGenerator::JitterAndFork(
	Segment* seed,
	float forkProbNow,
	Segment* parent
)
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

		res.push_back(
			forkedSeg
		);
	}
		
	return res;
}
