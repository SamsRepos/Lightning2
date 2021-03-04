#include "SegmentRemoval.h"

void RemoveAllChildrenToBeCulled(std::vector<Segment*>* wholeVec);

void CullSegments(std::vector<Segment*>* vec)
{
	RemoveAllChildrenToBeCulled(vec);

	if (vec)
	{
		auto it = vec->begin();
		while (it != vec->end())
		{
			Segment* seg = *it;
									
			if (seg->IsToBeCulled())
			{
				it = vec->erase(it);
				delete seg;
			}
			else
			{
				it++;
			}
		}
	}
}

void RemoveAllChildrenToBeCulled(std::vector<Segment*>* wholeVec)
{


	if (wholeVec)
	{
		for (Segment* seg : *wholeVec)
		{
			std::vector<Segment*>* childrenVec = seg->GetChildren();

			if (childrenVec)
			{
				auto it = childrenVec->begin();
				while (it != childrenVec->end())
				{
					Segment* child = *it;

					if (child->IsToBeCulled())
					{
						it = childrenVec->erase(it);
					}
					else
					{
						it++;
					}
				}

			}

		}		
	}
}