#pragma once

#include "LineRenderer.h"
#include "CylinderRenderer.h"

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

	void UpdateAnimation(float dt);

	void Render(
		Camera* camera,
		const XMMATRIX& worldMatrix,
		const XMMATRIX& viewMatrix,
		const XMMATRIX& projMatrix
	);

	inline void SetBlurRenderingActive(bool active) { blurRenderingActive = active; };
	inline void SetLineRenderingActive(bool active) { lineRenderingActive = active; };
	inline void SetCylinderRenderingActive(bool active) { cylinderRenderingActive = active; };

	inline bool IsBlurRenderingActive() { return blurRenderingActive; };
	inline bool IsLineRenderingActive() { return lineRenderingActive; };
	inline bool IsCylinderRenderingActive() { return cylinderRenderingActive; };

private:
	D3D* renderer;
	LineRenderer* lineRenderer;
	CylinderRenderer* cylRenderer;

	bool blurRenderingActive;
	bool lineRenderingActive;
	bool cylinderRenderingActive;

	float animationSpeed;
};

