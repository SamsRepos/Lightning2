#include "PathIdentifier.h"

////
// PUBLIC:
////

void PathIdentifier::Run()
{
	Segment* root = segments->front();

	DescendantCounterRecurs(root);
	StatusSetterRecurs(root);
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

void PathIdentifier::StatusSetterRecurs(Segment* currentSegment)
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
			StatusSetterRecurs(child);
		}
	}
	
//#if 0
//	if (currentSegment->GetChildren()->size() == 2)
//	{
//		Segment* childA = currentSegment->GetChild(0);
//		Segment* childB = currentSegment->GetChild(1);
//
//		if (childA->GetNumDescendants() > childB->GetNumDescendants())
//		{
//			childA->SetStatus(SegmentStatuses::PRIMARY);
//			childB->SetStatus(SegmentStatuses::SECONDARY);
//		}
//		else
//		{
//			childB->SetStatus(SegmentStatuses::PRIMARY);
//			childA->SetStatus(SegmentStatuses::SECONDARY);
//		}
//
//		StatusSetterRecurs(childA);
//		StatusSetterRecurs(childB);
//	}
//	else if (currentSegment->GetChildren()->size() == 1)
//	{
//		//should only be true after jitter+fork method
//		currentSegment->GetChild(0)->SetStatus(SegmentStatuses::PRIMARY);
//		StatusSetterRecurs(currentSegment->GetChild(0));
//	}
//#endif
	
}