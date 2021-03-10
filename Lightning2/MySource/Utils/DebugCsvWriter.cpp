#include "DebugCsvWriter.h"

#include <fstream>
#include <cassert>
#include <map>

void DebugWriteCsv(std::vector<Segment*>* segments, const char* filePath)
{
	//1. Initialising a map, where each segment is named with a number
	std::map<Segment*, int> intMap;
	int i = 0;
	for (Segment* segment : *segments)
	{
		intMap[segment] = i++;
	}
	
	//2. Initialse out file
	std::ofstream outFile(filePath);
	assert(outFile.is_open());

	//3. Output relevant data:

	//Column headings:
	outFile << "SEG NUM, PARENT, CHILDREN, STATUS, START X, START Y, START X, END X, END Y, END Z, LENGTH, ENERGY, VELOCITY, DIST FROM ROOT, FARTHEST DIST FROM ROOT, \n";

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

		//STATUS
		outFile << ((segment->GetStatus() == SegmentStatuses::PRIMARY) ? "PRIMARY" : "SECONDARY") << ", ";

		//START X
		outFile << segment->GetStartPoint().x << ", ";
		//START Y
		outFile << segment->GetStartPoint().y << ", ";
		//START Z
		outFile << segment->GetStartPoint().z << ", ";
		//END X
		outFile << segment->GetEndPoint().x << ", ";
		//END Y
		outFile << segment->GetEndPoint().y << ", ";
		//END Z
		outFile << segment->GetEndPoint().z << ", ";
		
		//LENGTH
		outFile << segment->GetLength() << ", ";
		
		//ENERGY
		outFile << segment->GetEnergy() << ", ";

		//VELOCITY
		outFile << segment->GetVelocity() << ", ";

		//DIST 2 ROOT:
		outFile << segment->GetDistanceFromRoot() << ", ";

		//MAX DIST 2 ROOT:
		outFile << segment->GetFarthestDistanceOnThisPath() << ", ";



		outFile << '\n';
	}
}