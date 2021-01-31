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
	CylinderRenderer();
	~CylinderRenderer();

	void Init(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight);
	void Build(std::vector<Segment*>* segments);
	void SetShaderParams(
		const XMMATRIX& _viewMatrix,
		const XMMATRIX& _projectionMatrix,
		const XMFLOAT4& _colour
	);
	void SetBlurParameters(
		bool _blurActive,
		float _blurExtent,
		float _blurRange,
		XMFLOAT4 _backgroundColour
	);
	void Render(D3D* renderer, Camera* camera);

private:
	CylinderMesh* cylinderMesh;
	SceneObject* baseCylinder;
	std::vector<SceneObject> cylinderObjects;

	CylinderShader* mainShader;
	BlurShader* blurShader;
	TextureShader* textureShader;

	RenderTexture* blurRenderTexture1;
	RenderTexture* blurRenderTexture2;
	
	OrthoMesh* fullScreenMesh;

	int screenWidth;
	int screenHeight;

	bool blurActive;
	float blurExtent;
	float blurRange;
	XMFLOAT4 blurBackgroundColour;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMFLOAT4 colour;

	//debug stuff:
	int cylindersToRender;
};




