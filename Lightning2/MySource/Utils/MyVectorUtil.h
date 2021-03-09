#pragma once

#include <vector>

template<class T>
constexpr void DeleteAllVectorData(std::vector<T*>* vec)
{
	if (vec)
	{
		for (T* thing : *vec)
		{
			if (thing)
			{
				delete thing;
				thing = NULL;
			}
		}

		vec->clear();
	}
}