#include "StreamerGenerator.h"

#include "MyMath.h"
#include "MyMatrix44.h"

//#include <algorithm>

////
// PUBLIC:
////

StreamerGenerator::StreamerGenerator()
{
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
	size_t _maxNumLayers,
	AngleFixMethods _angleFixMethod,
	GasCompositions _gasComposition
)
{
	startPoint       = _startPoint;
	initDirection    = _initDirection.Normalised();
	voltage          = _voltage;
	initPressure     = _initPressure;
	pressureGradient = _pressureGradient;
	maxNumLayers     = _maxNumLayers;
	angleFixMethod   = _angleFixMethod;

	SetGasComposition(_gasComposition);
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

void StreamerGenerator::SetGasComposition(GasCompositions gasComposition)
{
	switch (gasComposition)
	{
	case(GasCompositions::AIR):
		diameterToLengthCoeffGen.SetMean(AIR_DIAMETER_TO_LENGTH_MEAN);
		diameterToLengthCoeffGen.SetStdDev(AIR_DIAMETER_TO_LENGTH_STDDEV);

		pressureToMinDiameterCoeffGen.SetMean(AIR_PRESSURE_TO_MIN_DIAMETER_MEAN);
		pressureToMinDiameterCoeffGen.SetStdDev(AIR_PRESSURE_TO_MIN_DIAMETER_STDDEV);
		break;
	case(GasCompositions::N2):
		diameterToLengthCoeffGen.SetMean(N2_DIAMETER_TO_LENGTH_MEAN);
		diameterToLengthCoeffGen.SetStdDev(N2_DIAMETER_TO_LENGTH_STDDEV);

		pressureToMinDiameterCoeffGen.SetMean(N2_PRESSURE_TO_MIN_DIAMETER_MEAN);
		pressureToMinDiameterCoeffGen.SetStdDev(N2_PRESSURE_TO_MIN_DIAMETER_STDDEV);
		break;
	}
}

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
		
		FixEndPoints(childA, childB);
				
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
	if (angleFixMethod == AngleFixMethods::NONE)
	{
		return;
	}

	// segA gets to keep its end-point, determined by the cone method
	// segB has its direction changed, relative to segA, using inner the angle
	
	float innerAngle = innerAngleGen.GetSample();
	
	// Try rotating with both possible axes:
	MyFloat3 rotationAxis1 = CrossProduct(segA->GetDirection(), segB->GetDirection()).Normalised();
	MyFloat3 rotationAxis2 = CrossProduct(segB->GetDirection(), segA->GetDirection()).Normalised();
	
	// Initially, each candidite end-point has the direction of segment a, with the magnitude of segment b itself
	MyFloat3 localEndPointB1 = (segA->GetDirection().Normalised() * segB->GetLength());
	MyFloat3 localEndPointB2 = localEndPointB1;

	MyMatrix44 rotationMatrix = RotationMatrix(rotationAxis1, innerAngle);
	localEndPointB1 = localEndPointB1 * rotationMatrix;

	rotationMatrix = RotationMatrix(rotationAxis2, innerAngle);
	localEndPointB2 = localEndPointB2 * rotationMatrix;
	
	MyFloat3 localEndPointB;

	switch (angleFixMethod)
	{
	case (AngleFixMethods::LOWEST_Y):
		localEndPointB =
			(localEndPointB1.y < localEndPointB2.y) ? 
			localEndPointB1 : 
			localEndPointB2;
		break;
	case (AngleFixMethods::FARTHEST_FROM_PARENT):
		Segment* parent = segB->GetParent();
		localEndPointB =
			((localEndPointB1 - parent->GetStartPoint()).Magnitude() > (localEndPointB2 - parent->GetStartPoint()).Magnitude()) ?
			localEndPointB1 :
			localEndPointB2;
		break;
	}

	segB->SetEndPoint(
		segB->GetStartPoint() + localEndPointB
	);
}
