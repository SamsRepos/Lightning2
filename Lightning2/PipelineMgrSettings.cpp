#include "PipelineMgrSettings.h"

PipelineMgrSettings::PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings)
	:
	currentGeometryGenerator(defaultSettings.geometryGenerator),
	diameterTransformerActive(defaultSettings.diameterTransformerActive),
	wholeTransformerActive(defaultSettings.wholeTransformerActive),
	electrifierActive(defaultSettings.electrifierActive),
	blurRenderingActive(defaultSettings.blurRenderingActive),
	lineRenderingActive(defaultSettings.lineRenderingActive),
    cylinderRenderingActive(defaultSettings.cylinderRenderingActive)
{
}