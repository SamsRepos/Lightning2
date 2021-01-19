#pragma once

enum GeometryGeneratorTypes
{
	JITTER_FORK,
	STREAMER
};

struct PipelineMgrDefaultSettings {
	GeometryGeneratorTypes geometryGenerator;
	bool diameterTransformerActive;
	bool wholeTransformerActive;
	bool electrifierActive;
};

class PipelineMgrSettings
{
public:
	PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings);

	inline void SetGeometryGenerator(GeometryGeneratorTypes type) { currentGeometryGenerator = type; };
	inline GeometryGeneratorTypes GetGeometryGenerator() { return currentGeometryGenerator; };

	inline void SetDiameterTransformerActive(bool active) { diameterTransformerActive = active; };
	inline void SetWholeTransformerActive(bool active) { wholeTransformerActive = active; };
	inline void SetElectifierActive(bool active) { electrifierActive = active; };

	inline bool IsPathIdentifierActive() { return (diameterTransformerActive || wholeTransformerActive); };
	inline bool IsDiameterTransformerActive() { return diameterTransformerActive; };
	inline bool IsWholeTransformerActive() { return wholeTransformerActive; };
	inline bool IsElectrifierActive() { return electrifierActive; };

private:
	GeometryGeneratorTypes currentGeometryGenerator;
	bool diameterTransformerActive;
	bool wholeTransformerActive;
	bool electrifierActive;
};
