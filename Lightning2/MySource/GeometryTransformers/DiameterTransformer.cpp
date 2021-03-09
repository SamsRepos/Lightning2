#include "DiameterTransformer.h"

#include "Utils/SegmentRemoval.h"

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

	CullSegments(segments);
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
				DiameterTransformRecurs(child, diameter, branchLevel);
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
		segment->SetToBeCulled();
	}
}
