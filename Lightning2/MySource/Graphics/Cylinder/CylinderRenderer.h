#pragma once

#include "CylinderMesh.h"
#include "CylinderObject.h"
#include "Shaders/CylinderShader.h"
#include "Shaders/BlurShader.h"
#include "Shaders/TextureShader.h"
#include "../AnimSegment.h"
#include "../RenderSettings.h"

class CylinderRenderer
{
public:
	CylinderRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight);
	~CylinderRenderer();

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
	void RenderCylinders(D3D* renderer, LightningRenderModes renderMode);
	
	void ClearCylinders();

private:
	float MaxEnergy(std::vector<AnimSegment*>* animSegs);

	CylinderMesh* cylinderMesh;
	CylinderObject* baseCylinder;
	std::vector<CylinderObject*> cylinderObjects;

	BlurShader* blurShader;
	TextureShader* textureShader;
	CylinderShader* mainShader;

	RenderTexture* blurRenderTexture1;
	RenderTexture* blurRenderTexture2;
	
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




