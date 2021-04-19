#include "Electrifier.h"

#include <algorithm>

#include "Utils/MyVectorUtil.h"
#include "DefaultParameters.h"
#include "Maths/MyClamp.h"

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
	recursCapHit = false;

	previousSegments = new std::vector<Segment*>;
	
	for (Segment* segment : *segments)
	{
		previousSegments->push_back(new Segment(*segment));
	}
	
	bool anySegmentTooLong;
	
	do
	{
		currentSegments = new std::vector<Segment*>;

		Segment* rootSeed = previousSegments->front();

		anySegmentTooLong = RunIterationRecurs(rootSeed, NULL, 0);
						
		//prep for the next iteration:
		DeleteAllVectorData(previousSegments);
		delete previousSegments;
		previousSegments = currentSegments;

	} while(anySegmentTooLong && !recursCapHit);

	//output vector:
	if (segments)
	{
		DeleteAllVectorData(segments);

		for (Segment* segPtr : *currentSegments)
		{
			segments->push_back(segPtr);
		}
	}
	
	// cleaning up working vectors:
	//note: not calling DeleteAllVectorData(currentSegments) because the dynamically allocated segments are now in segments
	//note: currentSegments and previousSegments now point to the same vector
	delete currentSegments;
	currentSegments = NULL;
	previousSegments = NULL;
}

////
// PRIVATE:
////

// root-to-end-effectors recursive
// returns true if any segment was too long
// sets recursCapHit flag, if recursCount reaches RECURSIVE_CAP
bool Electrifier::RunIterationRecurs(Segment* seed, Segment* parentSegment, size_t recursCount)
{
	Segment* nextParent;
	bool anySegTooLong;

	if (seed->GetLength() > maxLength)
	{
		std::vector<Segment*> res = Jitter(seed, parentSegment);
		
		currentSegments->insert(currentSegments->end(), res.begin(), res.end());

		nextParent = res[1]; // this is bottomSeg

		anySegTooLong = true;
	}
	else
	{
		Segment* newSeedCopy = new Segment(
			seed->GetStartPoint(),
			seed->GetEndPoint(),
			seed->GetDiameter()
		);

		newSeedCopy->SetEnergy(seed->GetEnergy());
		newSeedCopy->SetVelocity(seed->GetVelocity());
		newSeedCopy->SetStatus(seed->GetStatus());
		
		if (parentSegment)
		{
			newSeedCopy->SetParent(parentSegment);
			parentSegment->AddChild(newSeedCopy);
		}
		
		currentSegments->push_back(newSeedCopy);
		
		nextParent = newSeedCopy;

		anySegTooLong = false;
	}

	if (recursCount < RECURSIVE_CAP)
	{
		for (Segment* seedChild : *(seed->GetChildren()))
		{
			anySegTooLong = RunIterationRecurs(seedChild, nextParent, recursCount+1) || anySegTooLong;
		}
	}
	else
	{
		recursCapHit = true;
	}
	
	return anySegTooLong;
}

std::vector<Segment*> Electrifier::Jitter(Segment* seed, Segment* parent)
{
	//1. get a random vector
	MyFloat3 randvec = RandomNormalisedVector();

	//2. get the normalised cross product of the current segment's dir vector, and a random vector
	MyFloat3 offset = CrossProduct(randvec, seed->GetDirection()).Normalised();
	//3. multiply that by by chaos factor
	float chaos = seed->GetLength() * chaosGaussianGen.GetSample();
	offset = offset * chaos;

	//4. get new point
	MyFloat3 newPt = seed->GetMidpoint() + offset;

	//5. two resulting segments
	Segment* topSeg    = new Segment(seed->GetStartPoint(), newPt, seed->GetDiameter());
	Segment* bottomSeg = new Segment(newPt, seed->GetEndPoint(), seed->GetDiameter());

	// Parenting:
	if(parent)
	{
		topSeg->SetParent(parent);
		parent->AddChild(topSeg);
	}
	bottomSeg->SetParent(topSeg);
	topSeg->AddChild(bottomSeg);

	// Other things to be sure to inherit from the seed:
	topSeg->SetEnergy(seed->GetEnergy());
	bottomSeg->SetEnergy(seed->GetEnergy());

	topSeg->SetVelocity(seed->GetVelocity());
	bottomSeg->SetVelocity(seed->GetVelocity());

	topSeg->SetStatus(seed->GetStatus());
	bottomSeg->SetStatus(seed->GetStatus());
	
	std::vector<Segment*> res = { topSeg, bottomSeg };

	return res;
}
