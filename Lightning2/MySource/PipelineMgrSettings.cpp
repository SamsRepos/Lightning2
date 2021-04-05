#include "PipelineMgrSettings.h"

PipelineMgrSettings::PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings)
{
	Init(defaultSettings);
}

void PipelineMgrSettings::Init(const PipelineMgrDefaultSettings& defaultSettings)
{
	currentGeometryGenerator  = defaultSettings.geometryGenerator;
	wholeTransformerActive    = defaultSettings.wholeTransformerActive;
	branchifierActive         = defaultSettings.branchifierActive;
	diameterThinnerActive     = defaultSettings.diameterThinnerActive;
	electrifierActive         = defaultSettings.electrifierActive;
	renderingActive           = defaultSettings.renderingActive;
}