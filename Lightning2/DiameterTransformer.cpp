#include "DiameterTransformer.h"

////
// PUBLIC:
////

void DiameterTransformer::InitParameters(float _initialDiameter, float _diameterScaledown, size_t _maxNumBranchLevels)
{
	initialDiameter    = _initialDiameter;
	diamterScaledown   = _diameterScaledown;
	maxNumBranchLevels = _maxNumBranchLevels;
}

void DiameterTransformer::Run()
{
	Segment* root = segments->front();

	DiameterTransformRecurs(root, initialDiameter, 0);
}

////
// PRIVATE
////

void DiameterTransformer::DiameterTransformRecurs(Segment* segment, float diameter, size_t branchLevel)
{
	if (branchLevel < maxNumBranchLevels)
	{
		segment->SetDiameter(diameter);
		for (Segment* child : *(segment->GetChildren()))
		{
			if (child->GetStatus() == SegmentStatuses::PRIMARY)
			{
				DiameterTransformRecurs(child, diameter, (branchLevel + 1));
			}
			else if (child->GetStatus() == SegmentStatuses::SECONDARY)
			{
				DiameterTransformRecurs(child, (diameter * diamterScaledown), (branchLevel + 1));
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
		//delete and remove segment

	}
}