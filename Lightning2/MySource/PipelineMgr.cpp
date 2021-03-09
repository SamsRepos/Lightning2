#include "PipelineMgr.h"

#include "Utils/MyVectorUtil.h"
#include "Maths/MyClamp.h"

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
	segments(NULL),
	lightningRenderer(NULL)
{
	settings = new PipelineMgrSettings(defaultSettings);

	lightningRenderer = new LightningRenderer
	(
		renderer, hwnd, screenWidth, screenHeight
	);
}

PipelineMgr::~PipelineMgr()
{
	if (lightningRenderer)
	{
		delete lightningRenderer;
		lightningRenderer = NULL;
	}
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
	if (settings->IsRenderingActive())
	{
		lightningRenderer->Build(segments);
		lightningRenderer->InitAnimation();
	}	
}

void PipelineMgr::UpdateAnimation(float dt)
{
	lightningRenderer->UpdateAnimation(dt);
}

void PipelineMgr::RenderOutput(
	Camera* camera,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projMatrix	
)
{
	if (!(settings->IsRenderingActive()))
	{
		return;
	}

	lightningRenderer->Render(
		camera,
		worldMatrix,
		viewMatrix,
		projMatrix
	);
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