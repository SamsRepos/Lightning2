#pragma once

#include <vector>

#include "Segment.h"
#include "BoxMullerGen.h"

const float INIT_VOLTAGE_CONST = 0.00000001f;

const float DIAMETER_TO_LENGTH_MEAN   = 11.f;
const float DIAMETER_TO_LENGTH_STDDEV = 4.f;

const float MINIMUM_DIAMATER_COEFFICIENT = .2f;

const float SQRT_A_HALF = 0.70710678118f;

class StreamerGenerator
{
public:
	StreamerGenerator();
	void InitParameters(
		MyFloat3 _startPoint,
		MyFloat3 _initDirection,
		float _initVoltage,
		float _initPressure,
		float _pressureGradient,
		size_t _numSegmentsLimit
	);
	void Run();
	inline std::vector<Segment*>& GetOutput() { return output; };

private:
	void InitAlgorithm();

	Segment* BuildSegment(Segment* parent);
	void CreateChildren(Segment* parent);
	
	inline float DiameterToLength(const float& diameter) { return diameter * diameterToLengthGaussianGen.GetSample(); };
	inline float CalculateLocalPressure(const float& y) { return initPressure + (pressureGradient *  (y - startPoint.y)); };
	inline float PressureToMinDiameter(const float& pressure) { return MINIMUM_DIAMATER_COEFFICIENT * (1.f / pressure); };
	inline float CalculateDiameter(Segment* parent, const float& thisMinDiameter) { return SQRT_A_HALF * parent->GetDiameter() * (thisMinDiameter / parent->GetMinDiameter()) ; };

	MyFloat3 startPoint;
	MyFloat3 initDirection;
	float initVoltage;
	float initPressure;
	float pressureGradient;
	size_t numSegmentsLimit;
	size_t numSegments;

	BoxMullerGen diameterToLengthGaussianGen;

	std::vector<Segment*> output;
};

