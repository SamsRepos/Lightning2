#include "PipelineMgr.h"

#include "MyVectorUtil.h"
#include "MyClamp.h"

////
// PUBLIC:
////

PipelineMgr::PipelineMgr(const PipelineMgrDefaultSettings& defaultSettings) 
	:
	segments(NULL)
{
	settings = new PipelineMgrSettings(defaultSettings);
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
	size_t maxNumLayers
)
{
	streamGen.InitParameters(
		startPoint,
		initialDirection,
		initialVoltage,
		initialPressure,
		pressureGradient,
		maxNumLayers
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

//todo whole transformer

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
	D3D* renderer,
	HWND hwnd
)
{
	lineRenderer.Init(
		renderer,
		hwnd
	);
}

void PipelineMgr::InitCylinderRenderer(
	D3D* renderer,
	HWND hwnd,
	int screenWidth,
	int screenHeight
)
{
	cylRenderer.Init(
		renderer,
		hwnd,
		screenWidth,
		screenHeight
	);
}

void PipelineMgr::SetBlurParameters(float blurExtent, float blurRange)
{
	cylRenderer.SetBlurParameters(blurExtent, blurRange);
}

void PipelineMgr::RunProcess()
{
	InitProcess();

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

	if (settings->IsPathIdentifierActive())
	{
		pathIdentifier.SetSegments(segments);
		pathIdentifier.Run();
	}
	if (settings->IsDiameterTransformerActive())
	{
		diameterTransformer.SetSegments(segments);
		diameterTransformer.Run();
	}
	if (settings->IsWholeTransformerActive())
	{

	}
	if (settings->IsElectrifierActive())
	{
		electrifier.SetSegments(segments);
		electrifier.Run();		
	}

	lineRenderer.Build(segments);
	cylRenderer.Build(segments);
}

void PipelineMgr::RenderOutput(
	D3D* renderer,
	Camera* camera,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projMatrix	
)
{
	if (settings->IsLineRendererActive())
	{
		lineRenderer.SetShaderParams(worldMatrix, viewMatrix, projMatrix, LIGHTNING_WHITE);
		lineRenderer.Render(renderer);
	}

	if (settings->IsCylinderRendererActive())
	{
		cylRenderer.SetShaderParams(viewMatrix, projMatrix, LIGHTNING_WHITE);
		cylRenderer.Render(renderer, camera);
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