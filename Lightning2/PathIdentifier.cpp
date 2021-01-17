#include "PathIdentifier.h"

void PathIdentifier::Run()
{
	Segment* root = segments->front();

	DescendantCounterRecurs(root);
	StatusSetterRecurs(root);
}

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
	i'm here
}