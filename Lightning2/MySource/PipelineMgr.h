#pragma once

#include "PipelineMgrSettings.h"

#include "GeometryGenerators/JitterForkGenerator.h"
#include "GeometryGenerators/StreamerGenerator.h"
#include "GeometryGenerators/StreamerSettings.h"

#include "GeometryTransformers/DiameterThinner.h"
#include "GeometryTransformers/PathIdentifier.h"
#include "GeometryTransformers/Branchifier.h"
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

	// set current geometry generator
	inline void SetGeometryGeneratorType(GeometryGeneratorTypes type) { settings->SetGeometryGeneratorType(type); };

	// toggle post-generation transform stages
	inline void SetWholeTransformerActive(bool active) { settings->SetWholeTransformerActive(active); };
	inline void SetBranchifierActive(bool active) { settings->SetBranchifierActive(active); };
	inline void SetDiameterThinnerActive(bool active) { settings->SetDiameterThinnerActive(active); };
	inline void SetElectifierActive(bool active) { settings->SetElectifierActive(active); };

	// renderer building
	inline void SetRendererBuildingActive(bool active) { settings->SetRendererBuildingActive(active); };

	// setters for all parameters
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
	
	void InitWholeTransformer(
		MyFloat3 startPoint,
		MyFloat3 endPoint
	);

	void InitBranchifier(
		float initialDiameter,
		float diameterScaledown,
		float animationTime,
		size_t maxNumBranchLevels
	);

	void InitDiameterThinner(
		float scale
	);

	void InitElectrifier(
		float maxSegmentLength,
		float chaosMean,
		float chaosStdDev
	);
		
	// Rendering
	inline void SetBlurRenderingActive(bool active) { lightningRenderer->SetBlurRenderingActive(active); };
	inline void SetLineRenderingActive(bool active) { lightningRenderer->SetLineRenderingActive(active); };
	inline void SetCapsuleRenderingActive(bool active) { lightningRenderer->SetCapsuleRenderingActive(active); };

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

	inline void RebuildRendering() { BuildRendering(); };

	void Clear();

	bool WasRecursCapHit();

	std::string WhichStagesHitRecursCap();

private:
	void InitProcess();
	
	void BuildRendering();

	//Settings:
	PipelineMgrSettings* settings;

	//The lightning data:
	std::vector<Segment*>* segments;

	//Geometry generators:
	JitterForkGenerator jitForkGen;
	StreamerGenerator streamGen;

	//Geometry transformers:
	PathIdentifier pathIdentifier;
	WholeTransformer wholeTransformer;
	Branchifier branchifier;
	DiameterThinner diameterThinner;
	Electrifier electrifier;

	//Graphics renderers:
	LightningRenderer* lightningRenderer;
};

