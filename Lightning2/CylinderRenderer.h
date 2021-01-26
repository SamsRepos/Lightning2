#pragma once

#include "LightShader.h"
#include "CylinderMesh.h"
#include "SceneObject.h"
#include "Segment.h"

class CylinderRenderer
{
public:
	CylinderRenderer();
	~CylinderRenderer();

	void Init(D3D* renderer, ID3D11ShaderResourceView* texture);
	void Build(std::vector<Segment*>* segments);
	void SetShaderParams(
		const XMMATRIX& _viewMatrix,
		const XMMATRIX& _projectionMatrix,
		Light* _light
	);
	void Render(D3D* renderer, LightShader* shader);

private:
	CylinderMesh* cylinderMesh;
	SceneObject* baseCylinder;
	std::vector<SceneObject> cylinderObjects;

	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	Light* light;

	//debug stuff:
	int cylindersToRender;
};




