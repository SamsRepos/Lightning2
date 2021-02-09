#pragma once

#include <vector>

#include "Segment.h"
#include "BoxMullerGen.h"

const float SQRT_A_HALF = 0.70710678118f;

// VOLTAGE => INITIAL DIAMETER
//  Initial voltage coefficient
//  Bailey et al. say 10^-8
//  From graph: 40 kV => diameter of 1 mm
//              80 kV => diamater of 2.5 mm
//  From Briels et al. (2008b) (ambient air at 1 bar):
//   below 5kV:    no streamers emerge
//   5kV to 40kV:  positive streamers form
//   5kV to 20kV:  d = 0.2mm
//   20kV to 40kB: d increases by a factor of 6
const float DIAMETER_RISE = 3.f; //mm
const float VOLTAGE_RUN   = 100.f; //kV
const float INIT_VOLTAGE_COEFF = DIAMETER_RISE / VOLTAGE_RUN;

// DIAMETER => LENGTH
//  L / d = 11 +- 4 (Briels et al., 2008a)
const float DIAMETER_TO_LENGTH_MEAN   = 11.f;
const float DIAMETER_TO_LENGTH_STDDEV = 4.f;

// In ambient air, pressure * dmin = 0.20 +- 0.02 (Briels et al., 2008a)
const float PRESSURE_TO_MIN_DIAMETER_MEAN   = .2f;
const float PRESSURE_TO_MIN_DIAMETER_STDDEV = .02f;

//Could do:
// Add velocity for animating... v = 0.5 d^2 (Briels et al., 2008b)

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
		size_t _maxNumLayers
	);
	void Run();
	inline std::vector<Segment*>* GetOutput() { return output; };

private:
	void InitAlgorithm();

	void CreateChildren(Segment* parent, size_t parentLayer);
	Segment* CreateSegment(Segment* parent);
	void FixEndPoints(Segment* segA, Segment* segB);

	// DIAMETER => LENGTH
	inline float DiameterToLength(const float& diameter)
	{ 
		return diameter * diameterToLengthCoeffGen.GetSample(); 
	};
	
	// LOCAL PRESSURE
	inline float CalculateLocalPressure(const float& y)
	{ 
		return initPressure + (pressureGradient *  (y - startPoint.y)); 
	};
	
	// PRESSURE => MINIMUM DIAMETER
	inline float PressureToMinDiameter(const float& pressure)
	{ 
		return (1.f / pressure) * pressureToMinDiameterCoeffGen.GetSample();
	};
	
	// DIAMETER
	inline float CalculateDiameter(Segment* parent, const float& thisMinDiameter) 
	{
		return SQRT_A_HALF * parent->GetDiameter() * (thisMinDiameter / parent->GetMinDiameter()) ; 
	};



	void FixEndPoint(Segment* seg, float angle);

	MyFloat3 startPoint;
	MyFloat3 initDirection;
	float voltage;
	float initPressure;
	float pressureGradient;
	size_t maxNumLayers;
	size_t numLayers;

	BoxMullerGen diameterToLengthCoeffGen;
	BoxMullerGen pressureToMinDiameterCoeffGen;

	std::vector<Segment*>* output;
};

