#pragma once

#include "PipelineMgrSettings.h"

#include "GeometryGenerators/JitterForkGenerator.h"
#include "GeometryGenerators/StreamerGenerator.h"
#include "GeometryGenerators/StreamerSettings.h"

#include "GeometryTransformers/DiameterThinner.h"
#include "GeometryTransformers/PathIdentifier.h"
#include "GeometryTransformers/DiameterTransformer.h"
#include "GeometryTransformers/WholeTransformer.h"
#include "GeometryTransformers/Electrifier.h"

#include "Graphics/LightningRenderer.h"

class PipelineMgr
{
public:
	PipelineMgr(
		const PipelineMgrDefaultSettings& defaultSettings,
		D3D* renderer,
		HWND hwnd,
		int screenWidth,
		int screenHeight
	);
	~PipelineMgr();
	
	inline PipelineMgrSettings* GetSettings() { return settings; };

	//set current geometry generator
	inline void SetGeometryGeneratorType(GeometryGeneratorTypes type) { settings->SetGeometryGeneratorType(type); };

	//toggle post-generation transform stages
	inline void SetDiameterThinnerActive(bool active) { settings->SetDiameterThinnerActive(active); };
	inline void SetWholeTransformerActive(bool active) { settings->SetWholeTransformerActive(active); };
	inline void SetDiameterTransformerActive(bool active) { settings->SetDiameterTransformerActive(active); };
	inline void SetElectifierActive(bool active) { settings->SetElectifierActive(active); };

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
		size_t maxNumLayers,
		AngleFixMethods angleFixMethod,
		GasCompositions gasComposition
	);

	void InitDiameterThinner(
		float scale
	);

	void InitWholeTransformer(
		MyFloat3 startPoint,
		MyFloat3 endPoint
	);

	void InitDiameterTransformer(
		float initialDiameter,
		float diameterScaledown,
		size_t maxNumBranchLevels
	);

	void InitElectrifier(
		float maxSegmentLength,
		float chaosMean,
		float chaosStdDev
	);
		
	// Rendering
	inline void SetBlurRenderingActive(bool active) { lightningRenderer->SetBlurRenderingActive(active); };
	inline void SetLineRenderingActive(bool active) { lightningRenderer->SetLineRenderingActive(active); };
	inline void SetCylinderRenderingActive(bool active) { lightningRenderer->SetCylinderRenderingActive(active); };

	inline LightningRenderer* GetLightningRenderer() { return lightningRenderer; };

	//run whole process
	void RunProcess();
	std::vector<Segment*>* GetSegments() { return segments; };

	//graphics:
	void UpdateAnimation(float dt);

	void RenderOutput(
		Camera* camera,
		const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix,
		const XMMATRIX& projMatrix
	);

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
	DiameterThinner diameterThinner;
	PathIdentifier pathIdentifier;
	WholeTransformer wholeTransformer;
	DiameterTransformer diameterTransformer;
	Electrifier electrifier;

	//Graphics renderers:
	LightningRenderer* lightningRenderer;
};

