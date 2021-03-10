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
	branchifierActive         = defaultSettings.branchifierActive;
	electrifierActive         = defaultSettings.electrifierActive;
	renderingActive           = defaultSettings.renderingActive;
}