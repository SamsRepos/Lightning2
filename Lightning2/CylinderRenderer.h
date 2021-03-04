#pragma once

#include "CylinderShader.h"
#include "CylinderMesh.h"
#include "CylinderObject.h"
#include "Segment.h"
#include "BlurShader.h"
#include "TextureShader.h"

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

	void Build(
		std::vector<Segment*>* segments,
		float maxEnergy
	);

	void InitAnimation();
	bool UpdateAnimation(float dt); //returns true when animation is over

	void SetShaderParams(const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderBlur(D3D* renderer, Camera* camera);
	void RenderCylinders(D3D* renderer);
	

private:
	void CreateCylinderRecurs(Segment* seg, CylinderObject* parentCyl);

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

	float maxEnergy;

	bool animatingNow;
};




