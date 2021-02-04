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

	//Column headings:
	outFile << "SEG NUM, PARENT, CHILDREN, DIST 2 ROOT, MAX DIST 2 ROOT, \n";

	for (Segment* segment : *segments)
	{
		//SEG NUM
		outFile << intMap[segment] << ", ";

		//PARENT
		if (segment->GetParent())
		{
			outFile << intMap[segment->GetParent()] << ", ";
		}
		else
		{
			outFile << "NO PARENT, ";
		}

		//CHILDREN
		if (segment->GetChildren()->size() > 0)
		{
			for (Segment* child : *(segment->GetChildren()))
			{
				outFile << ((child->GetStatus() == SegmentStatuses::PRIMARY) ? "pr: " : "sec: ");
				outFile << intMap[child] << " ";
			}
			outFile << ", ";
		}
		else
		{
			outFile << "NO CHILDREN, ";
		}

		//DIST 2 ROOT:
		outFile << segment->GetDistanceFromRoot() << ", ";

		//MAX DIST 2 ROOT:
		outFile << segment->GetFarthestDistanceOnThisPath() << ", ";


		outFile << '\n';
	}
}