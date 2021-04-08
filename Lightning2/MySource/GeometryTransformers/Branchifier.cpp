#include "Branchifier.h"

#include <algorithm>

#include "Utils/SegmentRemoval.h"

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
	Segment* root = segments->front();

	BranchifyRecurs(
		root,
		initialDiameter,
		root->GetFarthestDistanceOnThisPath() / animationTime,
		0
	);

	CullSegments(segments);
}

////
// PRIVATE
////

void Branchifier::BranchifyRecurs(Segment* segment, float diameter, float velocity, size_t branchLevel)
{
	if (branchLevel < maxNumBranchLevels)
	{
		segment->SetDiameter(diameter);
		segment->SetVelocity(velocity);
		
		for (Segment* child : *(segment->GetChildren()))
		{
			if (child->GetStatus() == SegmentStatuses::PRIMARY)
			{
				BranchifyRecurs(child, diameter, velocity, branchLevel);
			}
			else if (child->GetStatus() == SegmentStatuses::SECONDARY)
			{
				BranchifyRecurs(
					child,
					(diameter * diameterScaledown),
					velocity * CalculateVelocityScaledown(segment, child),
					(branchLevel + 1)
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
		segment->SetToBeCulled();
	}
}

float Branchifier::CalculateVelocityScaledown(Segment* segment, Segment* child)
{
	float remainingDistanceOnThisPath = segment->GetFarthestDistanceOnThisPath() - segment->GetDistanceFromRoot();
	float remainingDistanceOnChildPath = child->GetFarthestDistanceOnThisPath() - child->GetDistanceFromRoot();

	float velocityScaledown = remainingDistanceOnChildPath / remainingDistanceOnThisPath;

	return std::max(0.f, velocityScaledown);
}
