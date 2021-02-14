#include "PipelineMgrSettings.h"

PipelineMgrSettings::PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings)
{
	Init(defaultSettings);
}

void PipelineMgrSettings::Init(const PipelineMgrDefaultSettings& defaultSettings)
{
	currentGeometryGenerator  = defaultSettings.geometryGenerator;
	diameterThinnerActive     = defaultSettings.diameterThinnerActive;
	wholeTransformerActive    = defaultSettings.wholeTransformerActive;
	diameterTransformerActive = defaultSettings.diameterTransformerActive;
	electrifierActive         = defaultSettings.electrifierActive;
	blurRenderingActive       = defaultSettings.blurRenderingActive;
	lineRenderingActive       = defaultSettings.lineRenderingActive;
	cylinderRenderingActive   = defaultSettings.cylinderRenderingActive;
}