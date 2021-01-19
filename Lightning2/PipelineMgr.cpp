#include "PipelineMgr.h"

#include "SegmentCuller.h"
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
	float chaosProportionToLength,
	float forkProbability,
	float forkProbabilityScaledown
)
{
	jitForkGen.InitParameters(
		Segment(startPoint, endPoint),
		iterations,
		chaosProportionToLength,
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
	float chaosProportionToLength
)
{
	electrifier.InitParameters(
		maxSegmentLength,
		chaosProportionToLength
	);
}

void PipelineMgr::RunProcess()
{
	InitProcess();

	switch (settings->GetGeometryGenerator())
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
		electrifier.GetOutput
	}
}

////
// PRIVATE:
////

void PipelineMgr::InitProcess()
{
	if (segments)
	{
		ClearAllSegmentData(segments);
	}
}