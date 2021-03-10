#pragma once

enum GeometryGeneratorTypes
{
	JITTER_FORK,
	STREAMER
};

struct PipelineMgrDefaultSettings {
	GeometryGeneratorTypes geometryGenerator;
	bool diameterThinnerActive;
	bool wholeTransformerActive;
	bool branchifierActive;
	bool electrifierActive;
	bool renderingActive;	
};

class PipelineMgrSettings
{
public:
	PipelineMgrSettings(const PipelineMgrDefaultSettings& defaultSettings);
	
	void Init(const PipelineMgrDefaultSettings& defaultSettings);

	inline void SetGeometryGeneratorType(GeometryGeneratorTypes type) { currentGeometryGenerator = type; };
	inline GeometryGeneratorTypes GetGeometryGeneratorType() { return currentGeometryGenerator; };

	inline void SetDiameterThinnerActive(bool active) { diameterThinnerActive = active; };
	inline void SetWholeTransformerActive(bool active) { wholeTransformerActive = active; };
	inline void SetBranchifierActive(bool active) { branchifierActive = active; };
	inline void SetElectifierActive(bool active) { electrifierActive = active; };
	inline void SetRenderingActive(bool active) { renderingActive = active; };

	inline bool IsDiameterThinnerActive() { return diameterThinnerActive; };
	inline bool IsPathIdentifierActive() { return ( branchifierActive || wholeTransformerActive); };
	inline bool IsWholeTransformerActive() { return wholeTransformerActive; };
	inline bool IsBranchifierActive() { return branchifierActive; };
	inline bool IsElectrifierActive() { return electrifierActive; };
	inline bool IsRenderingActive() { return renderingActive; };

private:
	GeometryGeneratorTypes currentGeometryGenerator;
	bool diameterThinnerActive;
	bool wholeTransformerActive;
	bool branchifierActive;
	bool electrifierActive;
	bool renderingActive;
};
