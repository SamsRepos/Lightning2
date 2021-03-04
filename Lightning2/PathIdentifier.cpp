#include "PathIdentifier.h"

////
// PUBLIC:
////

void PathIdentifier::Run()
{
	Segment* root = segments->front();

	/*for (Segment* seg : *segments)
	{
		seg->SetDistanceFromRoot(root);
	}*/

	FarthestDistOnPathSetterRecurs(root, root);
	StatusSetterRecurs(root);
}

////
// PRIVATE:
////

void PathIdentifier::FarthestDistOnPathSetterRecurs(Segment* root, Segment* currentSegment)
{	
	if (currentSegment->GetChildren()->size() > 0)
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			FarthestDistOnPathSetterRecurs(root, child);
		}

		float greatestDistToRootOnThisPath = -1.f;
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			if (child->GetFarthestDistanceOnThisPath() > greatestDistToRootOnThisPath)
			{
				greatestDistToRootOnThisPath = child->GetFarthestDistanceOnThisPath();
			}
		}
		currentSegment->SetFarthestDistanceOnThisPath(
			greatestDistToRootOnThisPath
		);
	}
	else
	{
		currentSegment->SetDistanceFromRoot(root);

		currentSegment->SetFarthestDistanceOnThisPath(
			currentSegment->GetDistanceFromRoot()
		);
	}
}

void PathIdentifier::StatusSetterRecurs(Segment* currentSegment)
{

	if (currentSegment->GetChildren()->size() > 0)
	{
		Segment* primaryChild = currentSegment->GetChild(0);
		float currentGreatestDistFromRoot = primaryChild->GetFarthestDistanceOnThisPath();

		for (size_t i = 1; i < currentSegment->GetChildren()->size(); i++)
		{
			Segment* currentChild = currentSegment->GetChild(i);
			if (currentChild->GetFarthestDistanceOnThisPath() > currentGreatestDistFromRoot)
			{
				currentGreatestDistFromRoot = currentChild->GetFarthestDistanceOnThisPath();
				primaryChild = currentChild;
			}
		}

		for (Segment* child : *(currentSegment->GetChildren()))
		{
			child->SetStatus((child == primaryChild) ? SegmentStatuses::PRIMARY : SegmentStatuses::SECONDARY);
			StatusSetterRecurs(child);
		}
	}
	
}
