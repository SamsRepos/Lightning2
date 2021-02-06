#pragma once

#include "PipelineMgrSettings.h"

#include "JitterForkGenerator.h"
#include "StreamerGenerator.h"

#include "PathIdentifier.h"
#include "DiameterTransformer.h"
#include "WholeTransformer.h"
#include "Electrifier.h"

#include "LineRenderer.h"
#include "CylinderRenderer.h"

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

	void InitWholeTransformer(
		MyFloat3 startPoint,
		MyFloat3 endPoint
	);

	void InitElectrifier(
		float maxSegmentLength,
		float chaosMean,
		float chaosStdDev
	);

	void InitLineRenderer(
		const XMFLOAT4& lineColour
	);

	void InitCylinderRenderer(
		const XMFLOAT4& blurColour,
		const XMFLOAT4& blurBackgroundColour,
		const XMFLOAT4& cylinderColour,
		float blurExtent,
		float blurRange
	);

	inline PipelineMgrSettings* GetSettings() { return settings; };

	//set current geometry generator
	inline void SetGeometryGeneratorType(GeometryGeneratorTypes type) { settings->SetGeometryGeneratorType(type); };
	
	//toggle post-generation transform stages
	inline void SetDiameterTransformerActive(bool active) { settings->SetDiameterTransformerActive(active); };
	inline void SetWholeTransformerActive(bool active) { settings->SetWholeTransformerActive(active); };
	inline void SetElectifierActive(bool active) { settings->SetElectifierActive(active); };

	//set current renderer
	inline void SetBlurRenderingActive(bool active) { settings->SetBlurRenderingActive(active); };
	inline void SetLineRenderingActive(bool active) { settings->SetLineRenderingActive(active); };
	inline void SetCylinderRenderingActive(bool active) { settings->SetCylinderRenderingActive(active); };

	//run whole process
	void RunProcess();
	std::vector<Segment*>* GetSegments() { return segments; };

	//render output
	void RenderOutput(
		D3D* renderer,
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
	PathIdentifier pathIdentifier;
	DiameterTransformer diameterTransformer;
	WholeTransformer wholeTransformer;
	Electrifier electrifier;

	//Graphics renderers:
	LineRenderer* lineRenderer;
	CylinderRenderer* cylRenderer;
};

