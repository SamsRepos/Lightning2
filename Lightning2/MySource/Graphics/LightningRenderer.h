#pragma once

#include "Line/LineRenderer.h"
#include "Cylinder/CylinderRenderer.h"
#include "AnimSegment.h"
#include "RenderSettings.h"

class LightningRenderer
{
public:
	LightningRenderer(
		D3D* renderer,
		HWND hwnd,
		int screenWidth,
		int screenHeight
	);
	~LightningRenderer();

	inline void SetRenderMode(LightningRenderModes mode) { renderMode = mode; };
	inline LightningRenderModes GetRenderMode() { return renderMode; };

	void SetColours(
		const XMFLOAT4& backgroundColour,
		const XMFLOAT4& blurColour,
		const XMFLOAT4& lineColour,
		const XMFLOAT4& cylinderColour
	);

	void SetBlurParams(
		float blurDirections,
		float blurQuality,
		float blurSize,
		float blurAdjustment
	);

	void SetAnimationParams(
		float speed
	);

	void Build(std::vector<Segment*>* segs);

	void InitAnimation();
	bool UpdateAnimation(float dt);

	void Render(
		Camera* camera,
		const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix,
		const XMMATRIX& projMatrix
	);

	void Clear();
		
	inline void SetBlurRenderingActive(bool active) { blurRenderingActive = active; };
	inline void SetLineRenderingActive(bool active) { lineRenderingActive = active; };
	inline void SetCylinderRenderingActive(bool active) { cylinderRenderingActive = active; };

	inline bool IsBlurRenderingActive() { return blurRenderingActive; };
	inline bool IsLineRenderingActive() { return lineRenderingActive; };
	inline bool IsCylinderRenderingActive() { return cylinderRenderingActive; };

private:
	void CreateAnimSegmentsRecurs(Segment* seg, AnimSegment* parent);
	
	std::vector<AnimSegment*> animSegments;

	D3D* renderer;
	LineRenderer* lineRenderer;
	CylinderRenderer* cylRenderer;

	LightningRenderModes renderMode;
	bool blurRenderingActive;
	bool lineRenderingActive;
	bool cylinderRenderingActive;
	
	bool animatingNow;
	float animationSpeed;
};
