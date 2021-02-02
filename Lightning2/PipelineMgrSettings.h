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
	bool blurRenderingActive;
	bool lineRenderingActive;
	bool cylinderRenderingActive;
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

	inline void SetBlurRenderingActive(bool active) { blurRenderingActive = active; };
	inline void SetLineRenderingActive(bool active) { lineRenderingActive = active; };
	inline void SetCylinderRenderingActive(bool active) { cylinderRenderingActive = active; };

	inline bool IsBlurRenderingActive() { return blurRenderingActive; };
	inline bool IsLineRenderingActive() { return lineRenderingActive; };
	inline bool IsCylinderRenderingActive() { return cylinderRenderingActive; };

private:
	GeometryGeneratorTypes currentGeometryGenerator;
	bool diameterTransformerActive;
	bool wholeTransformerActive;
	bool electrifierActive;
	bool blurRenderingActive;
	bool lineRenderingActive;
	bool cylinderRenderingActive;
};
