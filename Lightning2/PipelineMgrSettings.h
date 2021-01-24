#pragma once

enum GeometryGeneratorTypes
{
	JITTER_FORK,
	STREAMER
};

enum RendererTypes
{
	LINE,
	CYLINDER
};

struct PipelineMgrDefaultSettings {
	GeometryGeneratorTypes geometryGenerator;
	bool diameterTransformerActive;
	bool wholeTransformerActive;
	bool electrifierActive;
	RendererTypes renderer;
};

class PipelineMgrSettings
{
public:
	PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings);

	inline void SetGeometryGeneratorType(GeometryGeneratorTypes type) { currentGeometryGenerator = type; };
	inline GeometryGeneratorTypes GetGeometryGeneratorType() { return currentGeometryGenerator; };

	inline void SetDiameterTransformerActive(bool active) { diameterTransformerActive = active; };
	inline void SetWholeTransformerActive(bool active) { wholeTransformerActive = active; };
	inline void SetElectifierActive(bool active) { electrifierActive = active; };

	inline bool IsPathIdentifierActive() { return (diameterTransformerActive || wholeTransformerActive); };
	inline bool IsDiameterTransformerActive() { return diameterTransformerActive; };
	inline bool IsWholeTransformerActive() { return wholeTransformerActive; };
	inline bool IsElectrifierActive() { return electrifierActive; };

	inline void SetRendererType(RendererTypes type) { currentRenderer = type; };
	inline RendererTypes GetRendererType() { return currentRenderer; };

private:
	GeometryGeneratorTypes currentGeometryGenerator;
	bool diameterTransformerActive;
	bool wholeTransformerActive;
	bool electrifierActive;
	RendererTypes currentRenderer;
};
