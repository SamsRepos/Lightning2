#include "PipelineMgrSettings.h"

PipelineMgrSettings::PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings)
	:
	currentGeometryGenerator(defaultSettings.geometryGenerator),
	diameterTransformerActive(defaultSettings.diameterTransformerActive),
	wholeTransformerActive(defaultSettings.wholeTransformerActive),
	electrifierActive(defaultSettings.electrifierActive),
	lineRendererActive(defaultSettings.lineRendererActive),
    cylinderRendererActive(defaultSettings.cylinderRendererActive)
{
}