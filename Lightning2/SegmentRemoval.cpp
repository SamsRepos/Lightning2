#include "SegmentRemoval.h"

void CullSegments(std::vector<Segment*>* vec)
{
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
