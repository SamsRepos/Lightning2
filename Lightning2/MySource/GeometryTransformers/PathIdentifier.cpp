#include "PathIdentifier.h"

////
// PUBLIC:
////

void PathIdentifier::Run()
{
	Segment* root = segments->front();
	
	RunRecurs(root, root);

	root->SetStatus(SegmentStatuses::PRIMARY);
}

////
// PRIVATE:
////

void PathIdentifier::RunRecurs(Segment* root, Segment* currentSegment)
{	
	// 1. Set each segement's euclidian distance from the root
	currentSegment->SetDistanceFromRoot(root);

	if (currentSegment->GetChildren()->size() > 0)
	{
		// Depth-first traversal, so getting the recursive call in first
		for (Segment* child : *(currentSegment->GetChildren()))
		{
			RunRecurs(root, child);
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
