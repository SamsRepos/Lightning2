#pragma once

#include "CylinderMesh.h"
#include "DomeMesh.h"
#include "CapsuleObject.h"
#include "Shaders/CapsuleShader.h"
#include "Shaders/BlurShader.h"
#include "Shaders/TextureShader.h"
#include "../AnimSegment.h"
#include "../RenderSettings.h"

class CapsuleRenderer
{
public:
	CapsuleRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight);
	~CapsuleRenderer();

	void SetColours(
		const XMFLOAT4& _backgroundColour,
		const XMFLOAT4& _blurColour,
		const XMFLOAT4& _cylinderColour		
	);

	void SetBlurParams(
		float _blurDirections,
		float _blurQuality,
		float _blurSize,
		float _blurFinalAdjustment
	);

	void Build(std::vector<AnimSegment*>* animSegs);

	void UpdateFromAnimation();

	void SetShaderParams(const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderBlur(D3D* renderer, Camera* camera, LightningRenderModes renderMode);
	void RenderCapsules(D3D* renderer, LightningRenderModes renderMode);
	
	void ClearCapsules();

private:
	float MaxEnergy(std::vector<AnimSegment*>* animSegs);
	
	inline bool ShouldBeRendered(LightningRenderModes renderMode, CapsuleObject* c) {
		return ((renderMode == LightningRenderModes::ANIMATED) && c->IsVisible()) ||
			   (renderMode == LightningRenderModes::STATIC);
	};

	CylinderMesh* cylinderMesh;
	DomeMesh* domeMesh;
	CapsuleObject* baseCapsule;
	std::vector<CapsuleObject*> capsuleObjects;

	BlurShader* blurShader;
	TextureShader* textureShader;
	CapsuleShader* mainShader;

	RenderTexture* blurRenderTexture1;
	RenderTexture* blurRenderTexture2;
	
	RenderTexture* energyTexture;

	OrthoMesh* fullScreenMesh;

	int screenWidth;
	int screenHeight;

	XMFLOAT4 blurColour;
	XMFLOAT4 cylinderColour;
	XMFLOAT4 backgroundColour;

	float blurDirections;
	float blurQuality;
	float blurSize;
	float blurFinalAdjustment;
	
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	
	bool animatingNow;	
};



