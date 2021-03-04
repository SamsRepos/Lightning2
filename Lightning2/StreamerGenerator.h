#pragma once

#include <vector>

#include "Segment.h"
#include "BoxMullerGen.h"
#include "StreamerSettings.h"

const float SQRT_A_HALF = 0.70710678118f;

// VOLTAGE => INITIAL DIAMETER
//  Initial voltage coefficient
//  Bailey et al. say 10^-8
//  From graph: 40 kV => diameter of 1 mm
//              80 kV => diamater of 2.5 mm
//  From Briels et al. (2008a) (ambient air at 1 bar):
//   below 5kV:    no streamers emerge
//   5kV to 40kV:  positive streamers form
//   5kV to 20kV:  d = 0.2mm
//   20kV to 40kB: d increases by a factor of 6
const float DIAMETER_RISE = 3.f; //mm
const float VOLTAGE_RUN   = 100.f; //kV
const float INIT_VOLTAGE_COEFF = DIAMETER_RISE / VOLTAGE_RUN;

// DIAMETER => LENGTH
//  In ambient air, L / d = 11 +- 4 (Briels et al., 2008b)
const float AIR_DIAMETER_TO_LENGTH_MEAN   = 11.f;
const float AIR_DIAMETER_TO_LENGTH_STDDEV = 4.f;
// In N2 Gas, L / d = 8 +- 4
const float N2_DIAMETER_TO_LENGTH_MEAN   = 8.f;
const float N2_DIAMETER_TO_LENGTH_STDDEV = 4.f;

// PRESSURE => MINIMUM DIAMETER
//  In ambient air, pressure * d_min = 0.20 +- 0.02 (Briels et al., 2008b)
const float AIR_PRESSURE_TO_MIN_DIAMETER_MEAN   = .2f;
const float AIR_PRESSURE_TO_MIN_DIAMETER_STDDEV = .02f;
// In N2 Gas, pressure * d_min = 0.12 += 0.03
const float N2_PRESSURE_TO_MIN_DIAMETER_MEAN   = .12f;
const float N2_PRESSURE_TO_MIN_DIAMETER_STDDEV = .03f;

// ANGLES (degrees)
const float DELTA_ANGLE_MEAN   = 35.f;
const float DELTA_ANGLE_STDDEV = 5.f;

const float INNER_ANGLE_MEAN   = 43.f;
const float INNER_ANGLE_STDDEV = 12.3f;

// ENERGY (J):
const float ENERGY_COEFF = 100000; // energy = 10^5 * L (Bailey et al., 2014)

// VELOCITY (mm/ns):
const float VELOCITY_COEFF = .5f; // v = 0.5 d^2 (Briels et al., 2008a)

// COULD DO:
// - Custom user gaussian gens, with arbitrary mean and std dev
// - User controls on overall streamer shape / direction

class StreamerGenerator
{
public:
	StreamerGenerator();
	void InitParameters(
		MyFloat3 _startPoint,
		MyFloat3 _initDirection,
		float _voltage,
		float _initPressure,
		float _pressureGradient,
		size_t _maxNumLayers,
		AngleFixMethods _angleFixMethod,
		GasCompositions _gasComposition
	);
	void Run();
	inline std::vector<Segment*>* GetOutput() { return output; };
	
private:
	void SetGasComposition(GasCompositions gasCompoisition);
	void InitAlgorithm();
	void CreateChildrenRecurs(Segment* parent, size_t parentLayer);
	Segment* CreateSegment(Segment* parent); // Creates a new segment, which is parallel to its parent

	// DIAMETER => LENGTH
	//  L / d = 11 +- 4 (Briels et al., 2008b)
	inline float DiameterToLength(const float& diameter)
	{ 
		return diameter * diameterToLengthCoeffGen.GetSample(); 
	};
	
	// LOCAL PRESSURE
	//  My relation: pressure = initial pressure + (pressure gradient * delta-Y)
	//  Vector from initial_y to start_y = this_y - initial_y
	//  But the negative vector is used, because the negative y-direction is the direction of the lightning path
	inline float CalculateLocalPressure(const float& y)
	{ 
		return initPressure + (pressureGradient *  -(y - startPoint.y)); 
	};
	
	// PRESSURE => MINIMUM DIAMETER
	// In ambient air, pressure * d_min = 0.20 +- 0.02 (Briels et al., 2008b)
	inline float PressureToMinDiameter(const float& pressure)
	{ 
		return (1.f / pressure) * pressureToMinDiameterCoeffGen.GetSample();
	};
	
	// PARENT'S DIAMETER AND MINIMUM DIAMETERS => DIAMETER
	//  d_new = sqrt(1/2) * d_parent * (d_min,new / d_min,parent)
	//  So, segments should get smaller but, ...
	//   ... 1. Get relatively smaller when moving into higher pressures
	//   ... 2. Get relitively larger when moving into lower pressures
	//   (Bailey et al., 2014)
	inline float CalculateDiameter(Segment* parent, const float& thisMinDiameter) 
	{
		return SQRT_A_HALF * parent->GetDiameter() * (thisMinDiameter / parent->GetMinDiameter()); 
	};

	// DIAMETER => VELOCITY:
	// v = 0.5 d^2 (Briels et al., 2008a)
	inline float CalculateVelocity(const float& diameter)
	{
		return VELOCITY_COEFF * (diameter * diameter);
	}

	// Moves end point using cone method
	void MoveEndPointToConeEdge(Segment* seg, float angle);

	// Gives new segments appropriate directions:
	void FixEndPoints(Segment* segA, Segment* segB);

	MyFloat3 startPoint;
	MyFloat3 initDirection;
	float voltage;
	float initPressure;
	float pressureGradient;
	size_t maxNumLayers;
	size_t numLayers;
	AngleFixMethods angleFixMethod;
	
	BoxMullerGen diameterToLengthCoeffGen;
	BoxMullerGen pressureToMinDiameterCoeffGen;

	BoxMullerGen deltaAngleGen;
	BoxMullerGen innerAngleGen;

	std::vector<Segment*>* output;
};

