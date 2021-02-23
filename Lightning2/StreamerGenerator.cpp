#include "StreamerGenerator.h"

#include "MyMath.h"

//#include <algorithm>

////
// PUBLIC:
////

StreamerGenerator::StreamerGenerator()
{
	diameterToLengthCoeffGen.SetMean(DIAMETER_TO_LENGTH_MEAN);
	diameterToLengthCoeffGen.SetStdDev(DIAMETER_TO_LENGTH_STDDEV);

	pressureToMinDiameterCoeffGen.SetMean(PRESSURE_TO_MIN_DIAMETER_MEAN);
	pressureToMinDiameterCoeffGen.SetStdDev(PRESSURE_TO_MIN_DIAMETER_STDDEV);

	deltaAngleGen.SetMean(DegToRad(DELTA_ANGLE_MEAN));
	deltaAngleGen.SetStdDev(DegToRad(DELTA_ANGLE_STDDEV));

	innerAngleGen.SetMean(DegToRad(INNER_ANGLE_MEAN));
	innerAngleGen.SetStdDev(DegToRad(INNER_ANGLE_STDDEV));
}

void StreamerGenerator::InitParameters(
	MyFloat3 _startPoint,
	MyFloat3 _initDirection,
	float _voltage,
	float _initPressure,
	float _pressureGradient,
	size_t _maxNumLayers
)
{
	startPoint       = _startPoint;
	initDirection    = _initDirection.Normalised();
	voltage          = _voltage;
	initPressure     = _initPressure;
	pressureGradient = _pressureGradient;
	maxNumLayers     = _maxNumLayers;
}

void StreamerGenerator::Run()
{
	InitAlgorithm();

	//algorithm
	float rootDiameter   = voltage * INIT_VOLTAGE_COEFF;
	float rootLength     = DiameterToLength(rootDiameter);
	MyFloat3 rootStartPt = startPoint;
	MyFloat3 rootEndPt   = startPoint + (initDirection * rootLength);

	float rootLocalPressure   = initPressure;
	float rootMinimumDiamater = PressureToMinDiameter(rootLocalPressure);

	Segment* rootSegment = new Segment(rootStartPt, rootEndPt, rootDiameter, rootMinimumDiamater);

	output->push_back(rootSegment);

	CreateChildrenRecurs(rootSegment, 0);
}

////
// PRIVATE:
////

void StreamerGenerator::InitAlgorithm()
{
	output = new std::vector<Segment*>;
	numLayers = 0;	
}

void StreamerGenerator::CreateChildrenRecurs(Segment* parent, size_t parentLayerNum)
{
	size_t thisLayerNum = parentLayerNum + 1;

	if(
		(parent->GetDiameter() > parent->GetMinDiameter()) &&
		thisLayerNum < maxNumLayers
	)
	{
		Segment* childA = CreateSegment(parent);
		Segment* childB = CreateSegment(parent);
		
		//FixEndPoints(childA, childB);
				
		CreateChildrenRecurs(childA, thisLayerNum);
		CreateChildrenRecurs(childB, thisLayerNum);
	}
}

// Creates a new segment, which is parallel to its parent:
Segment* StreamerGenerator::CreateSegment(Segment* parent)
{
	MyFloat3 thisStartPoint = parent->GetEndPoint();

	float localPressure = CalculateLocalPressure(thisStartPoint.y);
	float minDiameter   = PressureToMinDiameter(localPressure);
	float diameter      = CalculateDiameter(parent, minDiameter);

	MyFloat3 direction     = parent->GetDirection().Normalised() * DiameterToLength(diameter);
	MyFloat3 thisEndPoint  = thisStartPoint + direction;

	Segment* newSegment = new Segment(
		thisStartPoint,
		thisEndPoint,
		diameter,
		minDiameter
	);

	// Angle fix:
	float angle = deltaAngleGen.GetSample();
	MoveEndPointToConeEdge(newSegment, angle);
	
	// Parent/childing:
	newSegment->SetParent(parent);
	parent->AddChild(newSegment);

	output->push_back(newSegment);
	
	return newSegment;
}

// Moves end point using cone method
void StreamerGenerator::MoveEndPointToConeEdge(Segment* seg, float angle)
{
	//Preserving current length
	float length = seg->GetLength();

	//1. Bring back end point to Lcos(angle) from the start point
	seg->SetEndPoint(
		seg->GetStartPoint() + (seg->GetDirection().Normalised() * length * cos(angle))
	);

	//2. Move out end point by Lsin(angle) in a random direction, perpendicular to current direction
	seg->SetEndPoint(
		seg->GetEndPoint() + (RandomPerpendicularUnitVector(seg->GetDirection()) * length * sin(angle))
	);
}

void StreamerGenerator::FixEndPoints(Segment* segA, Segment* segB)
{
	// segA gets to keep its end-point, determined by the cone method
	// segB has its direction changed, relative to segA, using inner the angle
	
	float innerAngle = innerAngleGen.GetSample();
	MyFloat3 rotationAxis = CrossProduct(segA->GetDirection(), segB->GetDirection()).Normalised();
	
	segB->SetEndPoint(
		segB->GetStartPoint() + (segA->GetDirection().Normalised() * segB->GetLength())
	);


}

