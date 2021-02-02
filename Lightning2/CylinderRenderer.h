#pragma once

#include "CylinderShader.h"
#include "CylinderMesh.h"
#include "SceneObject.h"
#include "Segment.h"
#include "BlurShader.h"
#include "TextureShader.h"

class CylinderRenderer
{
public:
	CylinderRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight);
	~CylinderRenderer();

	void InitParameters(
		const XMFLOAT4& blurColour,
		const XMFLOAT4& blurBackgroundColour,
		const XMFLOAT4& cylinderColour,
		float blurExtent,
		float blurRange
	);
	void Build(std::vector<Segment*>* segments);
	void SetShaderParams(const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderBlur(D3D* renderer, Camera* camera);
	void RenderCylinders(D3D* renderer);

private:
	CylinderMesh* cylinderMesh;
	SceneObject* baseCylinder;
	std::vector<SceneObject> cylinderObjects;

	BlurShader* blurShader;
	TextureShader* textureShader;
	CylinderShader* mainShader;

	RenderTexture* blurRenderTexture1;
	RenderTexture* blurRenderTexture2;
	
	OrthoMesh* fullScreenMesh;

	int screenWidth;
	int screenHeight;

	XMFLOAT4 blurColour;
	XMFLOAT4 blurBackgroundColour;
	XMFLOAT4 cylinderColour;

	float blurExtent;
	float blurRange;
	
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	

	//debug stuff:
	int cylindersToRender;
};




