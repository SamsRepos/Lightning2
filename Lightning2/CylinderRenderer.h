#pragma once

#include "CylinderShader.h"
#include "CylinderMesh.h"
#include "SceneObject.h"
#include "Segment.h"

class CylinderRenderer
{
public:
	CylinderRenderer();
	~CylinderRenderer();

	void Init(D3D* renderer, HWND hwnd, ID3D11ShaderResourceView* texture);
	void Build(std::vector<Segment*>* segments);
	void SetShaderParams(
		const XMMATRIX& _viewMatrix,
		const XMMATRIX& _projectionMatrix,
		const XMFLOAT4& _colour
	);
	void Render(D3D* renderer);

private:
	CylinderMesh* cylinderMesh;
	SceneObject* baseCylinder;
	std::vector<SceneObject> cylinderObjects;

	CylinderShader* shader;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	XMFLOAT4 colour;

	//debug stuff:
	int cylindersToRender;
};




