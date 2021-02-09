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
	float rootDiameter = voltage * INIT_VOLTAGE_COEFF;
	float rootLength = DiameterToLength(rootDiameter);
	MyFloat3 rootStartPt = startPoint;
	MyFloat3 rootEndPt = startPoint + (initDirection * rootLength);

	float rootLocalPressure = initPressure;
	float rootMinimumDiamater = PressureToMinDiameter(rootLocalPressure);

	Segment* rootSegment = new Segment(rootStartPt, rootEndPt, rootDiameter, rootMinimumDiamater);

	output->push_back(rootSegment);

	CreateChildren(rootSegment, 0);
}

////
// PRIVATE:
////

void StreamerGenerator::InitAlgorithm()
{
	output = new std::vector<Segment*>;
	numLayers = 0;	
}

void StreamerGenerator::CreateChildren(Segment* parent, size_t parentLayerNum)
{
	size_t thisLayerNum = parentLayerNum + 1;

	if(
		(parent->GetDiameter() > parent->GetMinDiameter()) &&
		thisLayerNum < maxNumLayers
	)
	{
		Segment* childA = CreateSegment(parent);
		Segment* childB = CreateSegment(parent);
		
		FixEndPoints(childA, childB);
				
		CreateChildren(childA, thisLayerNum);
		CreateChildren(childB, thisLayerNum);

		//TODO fix: this is going to be one-sided due to recursion, if max segment limit is hit
	}
}

Segment* StreamerGenerator::CreateSegment(Segment* parent)
{
	MyFloat3 thisStartPoint = parent->GetEndPoint();

	float localPressure = CalculateLocalPressure(thisStartPoint.y);
	float minDiameter   = PressureToMinDiameter(localPressure);
	float diameter      = CalculateDiameter(parent, minDiameter);

	MyFloat3 direction = parent->GetDirection().Normalised() * DiameterToLength(diameter);
	MyFloat3 endPoint  = thisStartPoint + direction;

	Segment* newSegment = new Segment(
		thisStartPoint,
		endPoint,
		diameter,
		minDiameter
	);

	newSegment->SetParent(parent);
	parent->AddChild(newSegment);

	output->push_back(newSegment);
	
	return newSegment;
}

void StreamerGenerator::FixEndPoints(Segment* segA, Segment* segB)
{
	float angleA = DegToRad(35.f);
	float angleB = DegToRad(35.f);

	FixEndPoint(segA, angleA);
	FixEndPoint(segB, angleB);

}

void StreamerGenerator::FixEndPoint(Segment* seg, float angle)
{
	float length = seg->GetLength();

	//1. bring back end point to Lcos(angle) from the start point
	seg->SetEndPoint(
		seg->GetStartPoint() +
		(
			seg->GetDirection().Normalised() *  // direction normalised
			length * cos(angle)                 // magnitude
		)
	);

	//2. move out end point by Lsin(angle) in a random direction, perpendicular to direction
	seg->SetEndPoint(
		seg->GetEndPoint() +
		(
			RandomPerpendicularUnitVector(seg->GetDirection()) * // direction normalised
			length * sin(angle)                                  // magnitude
		)
	);
}