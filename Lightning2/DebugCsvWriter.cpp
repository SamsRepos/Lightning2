#include "DebugCsvWriter.h"

#include <fstream>
#include <cassert>
#include <map>

void DebugWriteCsv(std::vector<Segment*>* segments, std::string filePath)
{
	//1. Initialising a map, where each segment is named with a number
	std::map<Segment*, int> intMap;
	int i = 0;
	for (Segment* segment : *segments)
	{
		intMap[segment] = ++i;
	}
	
	//2. Initialse out file
	std::ofstream outFile(filePath);
	assert(outFile.is_open());

	//3. Output relevant data:
	for (Segment* segment : *segments)
	{
		outFile << "SEG: " << intMap[segment] << ", ";

		if (segment->GetParent())
		{
			outFile << "PAR: " << intMap[segment->GetParent()] << ", ";
		}
		else
		{
			outFile << "NO PAR, ";
		}

		if (segment->GetChildren()->size() > 0)
		{
			outFile << "CH: ";
			for (Segment* child : *(segment->GetChildren()))
			{
				outFile << intMap[child] << " ";
			}
			outFile << ", ";
		}
		else
		{
			outFile << "NO CH, ";
		}

		outFile << '\n';
	}
}