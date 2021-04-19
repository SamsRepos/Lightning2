#include "PathIdentifier.h"

#include "DefaultParameters.h"

////
// PUBLIC:
////

void PathIdentifier::Run()
{
	recursCapHit = false;

	Segment* root = segments->front();
	
	RunRecurs(root, root, 0.f, 0);

	root->SetStatus(SegmentStatuses::PRIMARY);
}

////
// PRIVATE:
////

void PathIdentifier::RunRecurs(Segment* root, Segment* currentSegment, float distFromRoot, size_t recursCount)
{	
	// 1. Set each segement's euclidian distance from the root
	distFromRoot += currentSegment->GetLength();
	currentSegment->SetDistanceFromRoot(distFromRoot);

	if (currentSegment->GetChildren()->size() > 0)
	{
		// Breadth-first traversal precedes this recursive call
		// Depth-first traversal follows this recursive call
		if (recursCount < RECURSIVE_CAP)
		{
			for (Segment* child : *(currentSegment->GetChildren()))
			{
				RunRecurs(root, child, distFromRoot, recursCount+1);
			}
		}
		else
		{
			recursCapHit = true;
		}

		float greatestDistToRootOnThisPath = -1.f;
		Segment* primaryChild = NULL;
		
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			if (child->GetFarthestDistanceOnThisPath() > greatestDistToRootOnThisPath)
			{
				greatestDistToRootOnThisPath = child->GetFarthestDistanceOnThisPath();
				primaryChild = child;
			}
		}

		// 2a. for segments with children:
		currentSegment->SetFarthestDistanceOnThisPath(
			greatestDistToRootOnThisPath
		);

		// 2b. set each child's status appropriately. 
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			child->SetStatus((child == primaryChild) ? SegmentStatuses::PRIMARY : SegmentStatuses::SECONDARY);
		}		
	}
	else // 2a. for end-effectors:
	{
		currentSegment->SetFarthestDistanceOnThisPath(
			currentSegment->GetDistanceFromRoot()
		);
	}
}

//void PathIdentifier::StatusSetterRecurs(Segment* currentSegment)
//{
//
//	if (currentSegment->GetChildren()->size() > 0)
//	{
//		Segment* primaryChild = currentSegment->GetChild(0);
//		float currentGreatestDistFromRoot = primaryChild->GetFarthestDistanceOnThisPath();
//
//		for (size_t i = 1; i < currentSegment->GetChildren()->size(); i++)
//		{
//			Segment* currentChild = currentSegment->GetChild(i);
//			if (currentChild->GetFarthestDistanceOnThisPath() > currentGreatestDistFromRoot)
//			{
//				currentGreatestDistFromRoot = currentChild->GetFarthestDistanceOnThisPath();
//				primaryChild = currentChild;
//			}
//		}
//
//		for (Segment* child : *(currentSegment->GetChildren()))
//		{
//			child->SetStatus((child == primaryChild) ? SegmentStatuses::PRIMARY : SegmentStatuses::SECONDARY);
//			StatusSetterRecurs(child);
//		}
//	}
//	
//}
