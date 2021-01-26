#pragma once

#include "PipelineMgrSettings.h"

#include "JitterForkGenerator.h"
#include "StreamerGenerator.h"

#include "PathIdentifier.h"
#include "DiameterTransformer.h"
#include "Electrifier.h"

class PipelineMgr
{
public:
	PipelineMgr(const PipelineMgrDefaultSettings& defaultSettings);

	//setters for all parameters
	void InitJitterForkGenerator(
		MyFloat3 startPoint,
		MyFloat3 endPoint,
		size_t iterations,
		float chaosProportionMean,
		float chaosProportionStdDev,
		float midpointStdDev,
		float forkProbability,
		float forkProbabilityScaledown
	);

	void InitStreamerGenerator(
		MyFloat3 startPoint,
		MyFloat3 initialDirection,
		float initialVoltage,
		float initialPressure,
		float pressureGradient,
		size_t maxNumLayers
	);

	void InitDiameterTransformer(
		float initialDiameter,
		float diameterScaledown,
		size_t maxNumBranchLevels
	);

	//todo whole transformer

	void InitElectrifier(
		float maxSegmentLength,
		float chaosMean,
		float chaosStdDev
	);

	inline PipelineMgrSettings* GetSettings() { return settings; };

	//set current geometry generator
	inline void SetGeometryGeneratorType(GeometryGeneratorTypes type) { settings->SetGeometryGeneratorType(type); };
	
	//toggle post-generation transform stages
	inline void SetDiameterTransformerActive(bool active) { settings->SetDiameterTransformerActive(active); };
	inline void SetWholeTransformerActive(bool active) { settings->SetWholeTransformerActive(active); };
	inline void SetElectifierActive(bool active) { settings->SetElectifierActive(active); };

	//set current renderer
	inline void SetLineRendererActive(bool active) { settings->SetLineRendererActive(active); };
	inline void SetCylinderRendererActive(bool active) { settings->SetCylinderRendererActive(active); };

	//run whole process
	void RunProcess();
	std::vector<Segment*>* GetSegments() { return segments; };

	//render output
	

private:

	void InitProcess();

	//Settings:
	PipelineMgrSettings* settings;

	//The lightning data:
	std::vector<Segment*>* segments;

	//Geometry generators:
	JitterForkGenerator jitForkGen;
	StreamerGenerator streamGen;

	//Geometry transformers:
	PathIdentifier pathIdentifier;
	DiameterTransformer diameterTransformer;
	//TODO whole output transformer
	Electrifier electrifier;

	//Graphics renderers:

};

