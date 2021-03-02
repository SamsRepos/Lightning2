#include "PipelineMgr.h"

#include "MyVectorUtil.h"
#include "MyClamp.h"

////
// PUBLIC:
////

PipelineMgr::PipelineMgr(
	const PipelineMgrDefaultSettings& defaultSettings,
	D3D* renderer,
	HWND hwnd,
	int screenWidth,
	int screenHeight
) 
	:
	segments(NULL)
{
	settings = new PipelineMgrSettings(defaultSettings);

	lineRenderer = new LineRenderer(renderer, hwnd);
	cylRenderer  = new CylinderRenderer(renderer, hwnd, screenWidth, screenHeight);
}

void PipelineMgr::InitJitterForkGenerator(
	MyFloat3 startPoint,
	MyFloat3 endPoint,
	size_t iterations,
	float chaosProportionMean,
	float chaosProportionStdDev,
	float midpointStdDev,
	float forkProbability,
	float forkProbabilityScaledown
)
{
	jitForkGen.InitParameters(
		Segment(startPoint, endPoint),
		iterations,
		chaosProportionMean,
		chaosProportionStdDev,
		midpointStdDev,
		forkProbability,
		forkProbabilityScaledown
	);
}

void PipelineMgr::InitStreamerGenerator(
	MyFloat3 startPoint,
	MyFloat3 initialDirection,
	float initialVoltage,
	float initialPressure,
	float pressureGradient,
	size_t maxNumLayers,
	AngleFixMethods angleFixMethod,
	GasCompositions gasComposition
)
{
	streamGen.InitParameters(
		startPoint,
		initialDirection,
		initialVoltage,
		initialPressure,
		pressureGradient,
		maxNumLayers,
		angleFixMethod,
		gasComposition
	);
}

void PipelineMgr::InitDiameterThinner(
	float scale
)
{
	diameterThinner.InitParameters(
		scale
	);
}

void PipelineMgr::InitWholeTransformer(
	MyFloat3  startPoint,
	MyFloat3 endPoint
)
{
	wholeTransformer.InitParameters(
		startPoint,
		endPoint
	);
}

void PipelineMgr::InitDiameterTransformer(
	float initialDiameter,
	float diameterScaledown,
	size_t maxNumBranchLevels
)
{
	diameterTransformer.InitParameters(
		initialDiameter,
		MyClamp(diameterScaledown, 0.f, 1.f),
		maxNumBranchLevels
	);
}

void PipelineMgr::InitElectrifier(
	float maxSegmentLength,
	float chaosMean,
	float chaosStdDev
)
{
	electrifier.InitParameters(
		maxSegmentLength,
		chaosMean,
		chaosStdDev
	);
}

void PipelineMgr::InitLineRenderer(
	const XMFLOAT4& lineColour
)
{
	lineRenderer->InitParameters(
		lineColour
	);
}

void PipelineMgr::InitCylinderRenderer(
	const XMFLOAT4& blurColour,
	const XMFLOAT4& blurBackgroundColour,
	const XMFLOAT4& cylinderColour,
	float blurDirections,
	float blurQuality,
	float blurSize,
	float blurAdjustment
)
{
	cylRenderer->InitParameters(
		blurColour,
		blurBackgroundColour,
		cylinderColour,
		blurDirections,
		blurQuality,
		blurSize,
		blurAdjustment
	);
}

void PipelineMgr::RunProcess()
{
	InitProcess();

	// Generating Geometry:
	switch (settings->GetGeometryGeneratorType())
	{
	case (GeometryGeneratorTypes::JITTER_FORK):
		jitForkGen.Run();
		segments = jitForkGen.GetOutput();
		break;
	case(GeometryGeneratorTypes::STREAMER):
		streamGen.Run();
		segments = streamGen.GetOutput();
		break;
	}

	// Transforming Geometry:
	if (settings->IsDiameterThinnerActive())
	{
		diameterThinner.SetSegments(segments);
		diameterThinner.Run();
	}
	if (settings->IsPathIdentifierActive())
	{
		pathIdentifier.SetSegments(segments);
		pathIdentifier.Run();
	}
	if (settings->IsWholeTransformerActive())
	{
		wholeTransformer.SetSegments(segments);
		wholeTransformer.Run();
	}
	if (settings->IsDiameterTransformerActive())
	{
		diameterTransformer.SetSegments(segments);
		diameterTransformer.Run();
	}	
	if (settings->IsElectrifierActive())
	{
		electrifier.SetSegments(segments);
		electrifier.Run();		
	}

	// Building Meshes:
	float maxEnergy = 0.f;
	for (Segment* seg : *segments)
	{
		if (seg->GetEnergy() > maxEnergy);
		maxEnergy = seg->GetEnergy();
	}

	lineRenderer->Build(segments);
	cylRenderer->Build(segments, maxEnergy);
}

void PipelineMgr::RenderOutput(
	D3D* renderer,
	Camera* camera,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projMatrix	
)
{

	if (settings->IsBlurRenderingActive() || settings->IsCylinderRenderingActive())
	{
		cylRenderer->SetShaderParams(viewMatrix, projMatrix);
	}

	if (settings->IsBlurRenderingActive())
	{
		cylRenderer->RenderBlur(renderer, camera);
	}

	if (settings->IsLineRenderingActive())
	{
		lineRenderer->SetShaderParams(worldMatrix, viewMatrix, projMatrix);
		lineRenderer->RenderLines(renderer);
	}

	if (settings->IsCylinderRenderingActive())
	{		
		cylRenderer->RenderCylinders(renderer);
	}
}

////
// PRIVATE:
////

void PipelineMgr::InitProcess()
{
	if(segments)
	{
		DeleteAllVectorData(segments);

		delete segments;
		segments = NULL;
	}
}