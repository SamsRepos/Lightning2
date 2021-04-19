#pragma once

#include "Line/LineRenderer.h"
#include "Capsule/CapsuleRenderer.h"
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

	void SetRenderMode(LightningRenderModes mode);
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

	void SetEnergyParams(
		EnergyScales scale,
		bool usedForBlur,
		bool usedForBrightness
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
	inline void SetCapsuleRenderingActive(bool active) { capsuleRenderingActive = active; };

	inline bool IsBlurRenderingActive() { return blurRenderingActive; };
	inline bool IsLineRenderingActive() { return lineRenderingActive; };
	inline bool IsCapsuleRenderingActive() { return capsuleRenderingActive; };

	inline bool WasRecursCapHit() { return recursCapHit; };

private:
	void CreateAnimSegmentsRecurs(Segment* seg, AnimSegment* parent, size_t recursCount);
	
	std::vector<AnimSegment*> animSegments;

	D3D* renderer;
	LineRenderer* lineRenderer;
	CapsuleRenderer* capRenderer;

	LightningRenderModes renderMode;
	bool blurRenderingActive;
	bool lineRenderingActive;
	bool capsuleRenderingActive;
	
	bool animatingNow;
	float animationSpeed;

	bool recursCapHit;
};
