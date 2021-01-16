#include "StreamerGenerator.h"


//#include <algorithm>

////
// PUBLIC:
////

StreamerGenerator::StreamerGenerator()
{
	diameterToLengthGaussianGen.SetMean(DIAMETER_TO_LENGTH_MEAN);
	diameterToLengthGaussianGen.SetStdDev(DIAMETER_TO_LENGTH_STDDEV);
}

void StreamerGenerator::InitParameters(
	MyFloat3 _startPoint,
	MyFloat3 _initDirection,
	float _initVoltage,
	float _initPressure,
	float _pressureGradient,
	size_t _numSegmentsLimit
)
{
	startPoint       = _startPoint;
	initDirection    = normalised(_initDirection);
	initVoltage      = _initVoltage;
	initPressure     = _initPressure;
	pressureGradient = _pressureGradient;
	numSegmentsLimit = _numSegmentsLimit;
}

void StreamerGenerator::Run()
{
	InitAlgorithm();

	//algorithm
	float rootDiameter = initVoltage * INIT_VOLTAGE_CONST;
	float rootLength = DiameterToLength(rootDiameter);
	MyFloat3 rootStartPt = startPoint;
	MyFloat3 rootEndPt = startPoint + (initDirection * rootLength);

	float rootLocalPressure = initPressure;
	float rootMinimumDiamater = PressureToMinDiameter(rootLocalPressure);

	Segment* rootSegment = new Segment(rootStartPt, rootEndPt, rootDiameter, rootMinimumDiamater);

	output.push_back(rootSegment);

	CreateChildren(rootSegment);
}

////
// PRIVATE:
////

void StreamerGenerator::InitAlgorithm()
{
	output.clear();
	numSegments = 0;	
}

Segment* StreamerGenerator::BuildSegment(Segment* parent)
{
	MyFloat3 startPoint = parent->GetEndPoint();

	float minDiameter = PressureToMinDiameter(CalculateLocalPressure(startPoint.y));
	float diameter    = CalculateDiameter(parent, minDiameter);

	return new Segment(
		startPoint,
		startPoint + initDirection * DiameterToLength(diameter),
		diameter,
		minDiameter
	);
}

void StreamerGenerator::CreateChildren(Segment* parent)
{
	if (parent->GetDiameter() > parent->GetMinDiameter())
	{
		Segment* childA = BuildSegment(parent);
		Segment* childB = BuildSegment(parent);

		output.push_back(childA);
		output.push_back(childB);

		//calculate directionas
		//use these to set desired end-point

		need to think through order of this
	}
}
