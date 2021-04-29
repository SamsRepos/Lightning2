#include "PipelineMgr.h"

#include <sstream>

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

void PipelineMgr::InitBranchifier(
	float initialDiameter,
	float diameterScaledown,
	float animationTime,
	size_t maxNumBranchLevels
)
{
	branchifier.InitParameters(
		initialDiameter,
		diameterScaledown,
		animationTime,
		maxNumBranchLevels
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
	case(GeometryGeneratorTypes::JITTER_FORK):
		jitForkGen.Run();
		segments = jitForkGen.GetOutput();
		break;
	case(GeometryGeneratorTypes::STREAMER):
		streamGen.Run();
		segments = streamGen.GetOutput();
		break;
	}

	// Transforming Geometry:	
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
	if (settings->IsBranchifierActive())
	{
		branchifier.SetSegments(segments);
		branchifier.Run();
	}
	if (settings->IsDiameterThinnerActive())
	{
		diameterThinner.SetSegments(segments);
		diameterThinner.Run();
	}
	if (settings->IsElectrifierActive())
	{
		electrifier.SetSegments(segments);
		electrifier.Run();		
	}

	// Building Meshes:
	if (settings->IsRendererBuildingActive())
	{
		BuildRendering();
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
	lightningRenderer->Render(
		camera,
		worldMatrix,
		viewMatrix,
		projMatrix
	);
}

void PipelineMgr::Clear()
{
	if (segments)
	{
		DeleteAllVectorData(segments);

		delete segments;
		segments = NULL;
	}

	lightningRenderer->Clear();
}

bool PipelineMgr::WasRecursCapHit()
{
	return (
		((settings->GetGeometryGeneratorType() == GeometryGeneratorTypes::JITTER_FORK) && jitForkGen.WasRecursCapHit()) ||
		((settings->GetGeometryGeneratorType() == GeometryGeneratorTypes::STREAMER) && streamGen.WasRecursCapHit()) ||
		(settings->IsPathIdentifierActive() && pathIdentifier.WasRecursCapHit()) ||
		(settings->IsWholeTransformerActive() && wholeTransformer.WasRecursCapHit()) ||
		(settings->IsBranchifierActive() && branchifier.WasRecursCapHit()) ||
		(settings->IsElectrifierActive() && electrifier.WasRecursCapHit()) ||
		lightningRenderer->WasRecursCapHit()
	);
}

std::string PipelineMgr::WhichStagesHitRecursCap()
{
	std::stringstream ss;
	if ((settings->GetGeometryGeneratorType() == GeometryGeneratorTypes::JITTER_FORK) && jitForkGen.WasRecursCapHit())
	{
		ss << "Jitter+Fork Generator, ";
	}
	if ((settings->GetGeometryGeneratorType() == GeometryGeneratorTypes::STREAMER) && streamGen.WasRecursCapHit())
	{
		ss << "Streamer Generator, ";
	}
	if (settings->IsPathIdentifierActive() && pathIdentifier.WasRecursCapHit())
	{
		ss << "Path Identifier, ";
	}
	if (settings->IsWholeTransformerActive() && wholeTransformer.WasRecursCapHit())
	{
		ss << "Whole Transformer, ";
	}
	if (settings->IsBranchifierActive() && branchifier.WasRecursCapHit())
	{
		ss << "Branchifier, ";
	}
	if (settings->IsElectrifierActive() && electrifier.WasRecursCapHit())
	{
		ss << "Electrifier, ";
	}
	if (lightningRenderer->WasRecursCapHit())
	{
		ss << "Lightning Renderer, ";
	}

	return ss.str();
}

////
// PRIVATE:
////

void PipelineMgr::InitProcess()
{
	Clear();	
}

void PipelineMgr::BuildRendering()
{
	lightningRenderer->Build(segments);
	lightningRenderer->InitAnimation();
}