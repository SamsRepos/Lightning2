#include "Branchifier.h"

#include <algorithm>

#include "Utils/SegmentRemoval.h"
#include "DefaultParameters.h"

////
// PUBLIC:
////

void Branchifier::InitParameters(
	float _initialDiameter,
	float _diameterScaledown,
	float _animationTime,
	size_t _maxNumBranchLevels
)
{
	initialDiameter     = _initialDiameter;
	diameterScaledown   = _diameterScaledown;
	animationTime       = _animationTime;
	maxNumBranchLevels  = _maxNumBranchLevels;
}

void Branchifier::Run()
{
	recursCapHit = false;

	Segment* root = segments->front();

	BranchifyRecurs(
		root,
		initialDiameter,
		root->GetFarthestDistanceOnThisPath() / animationTime,
		0,
		0
	);

	CullSegments(segments);
}

////
// PRIVATE
////

void Branchifier::BranchifyRecurs(Segment* segment, float diameter, float velocity, size_t branchLevel, size_t recursCount)
{
	if (branchLevel < maxNumBranchLevels)
	{
		segment->SetDiameter(diameter);
		segment->SetVelocity(velocity);
		
		if (recursCount < RECURSIVE_CAP)
		{
			for (Segment* child : *(segment->GetChildren()))
			{
				if (child->GetStatus() == SegmentStatuses::PRIMARY)
				{
					BranchifyRecurs(child, diameter, velocity, branchLevel, recursCount+1);
				}
				else if (child->GetStatus() == SegmentStatuses::SECONDARY)
				{
					BranchifyRecurs(
						child,
						(diameter * diameterScaledown),
						velocity * CalculateVelocityScaledown(segment, child),
						(branchLevel + 1),
						recursCount+1
					);
				}
				else
				{
					//invalid status
					return;
				}
			}
		}
		else
		{
			recursCapHit = true;
		}
	}
	else
	{
		segment->SetToBeCulled();
	}
}

float Branchifier::CalculateVelocityScaledown(Segment* segment, Segment* child)
{
	float remainingDistanceOnThisPath = segment->GetFarthestDistanceOnThisPath() - segment->GetDistanceFromRoot();
	float remainingDistanceOnChildPath = child->GetFarthestDistanceOnThisPath() - child->GetDistanceFromRoot();

	float velocityScaledown = remainingDistanceOnChildPath / remainingDistanceOnThisPath;

	return max(0.f, velocityScaledown);
}
