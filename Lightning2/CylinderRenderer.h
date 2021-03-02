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

	void InitParameters(
		const XMFLOAT4& _blurColour,
		const XMFLOAT4& _backgroundColour,
		const XMFLOAT4& _cylinderColour,
		float _blurExtent,
		float _blurRange
	);

	void Build(
		std::vector<Segment*>* segments,
		float maxEnergy
	);

	void SetShaderParams(const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderBlur(D3D* renderer, Camera* camera);
	void RenderCylinders(D3D* renderer);

private:
	CylinderMesh* cylinderMesh;
	CylinderObject* baseCylinder;
	std::vector<CylinderObject> cylinderObjects;

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

	float blurExtent;
	float blurRange;
	
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	float maxEnergy;
};




