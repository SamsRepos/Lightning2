#include "PathIdentifier.h"

////
// PUBLIC:
////

void PathIdentifier::Run()
{
	Segment* root = segments->front();

#if 1	
	DescendantCounterRecurs(root);
	StatusSetterRecursByDescendants(root);
#endif

#if 0
	DistFromRootSetterRecurs(root, root);
	StatusSetterRecursByDistFromRoot(root);
#endif
}

////
// PRIVATE:
////

void PathIdentifier::DescendantCounterRecurs(Segment* currentSegment)
{
	size_t numChildren = currentSegment->GetChildren()->size();

	// 1.
	if(numChildren > 0)
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			DescendantCounterRecurs(child);
		}
	}
	else
	{
		currentSegment->SetNumDescendants(0);
	}

	// 2. All descendants should have their num descendants now
	size_t numDescendants = numChildren;
	for (Segment* child : *(currentSegment->GetChildren()))
	{
		numDescendants += child->GetNumDescendants();
	}

	currentSegment->SetNumDescendants(numDescendants);
}

void PathIdentifier::StatusSetterRecursByDescendants(Segment* currentSegment)
{

	if (currentSegment->GetChildren()->size() > 0)
	{
		Segment* primaryChild = currentSegment->GetChild(0);
		int currentGreatestNumDescendants = primaryChild->GetNumDescendants();
		
		for (size_t i = 1; i < currentSegment->GetChildren()->size(); i++)
		{
			Segment* currentChild = currentSegment->GetChild(i);
			if (currentChild->GetNumDescendants() > currentGreatestNumDescendants)
			{
				currentGreatestNumDescendants = currentChild->GetNumDescendants();
				primaryChild = currentChild;
			}
		}

		for (Segment* child : *(currentSegment->GetChildren()))
		{
			child->SetStatus((child == primaryChild) ? SegmentStatuses::PRIMARY : SegmentStatuses::SECONDARY);
			StatusSetterRecursByDescendants(child);
		}
	}	
}

void PathIdentifier::DistFromRootSetterRecurs(Segment* root, Segment* currentSegment)
{
	currentSegment->SetDistanceFromRoot(root);

	size_t numChildren = currentSegment->GetChildren()->size();

	// 1.
	if (numChildren > 0)
	{
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			DistFromRootSetterRecurs(root, child);
		}


		// 2. All descendants should have their distance from root by now
		// and we're going back "up" the tree
		float greatestDistToRootOnThisPath = -1.f;
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			if (child->GetDistanceFromRoot() > greatestDistToRootOnThisPath)
			{
				greatestDistToRootOnThisPath = child->GetDistanceFromRoot();
			}
		}
		currentSegment->SetFarthestDistanceOnThisPath(
			greatestDistToRootOnThisPath
		);
	}
	else
	{
		currentSegment->SetFarthestDistanceOnThisPath(
			currentSegment->GetDistanceFromRoot()
		);
	}
}

void PathIdentifier::StatusSetterRecursByDistFromRoot(Segment* currentSegment)
{

	if (currentSegment->GetChildren()->size() > 0)
	{
		Segment* primaryChild = currentSegment->GetChild(0);
		float currentGreatestDistFromRoot = primaryChild->GetDistanceFromRoot();

		for (size_t i = 1; i < currentSegment->GetChildren()->size(); i++)
		{
			Segment* currentChild = currentSegment->GetChild(i);
			if (currentChild->GetDistanceFromRoot() > currentGreatestDistFromRoot)
			{
				currentGreatestDistFromRoot = currentChild->GetDistanceFromRoot();
				primaryChild = currentChild;
			}
		}

		for (Segment* child : *(currentSegment->GetChildren()))
		{
			child->SetStatus((child == primaryChild) ? SegmentStatuses::PRIMARY : SegmentStatuses::SECONDARY);
			StatusSetterRecursByDescendants(child);
		}
	}
	
}