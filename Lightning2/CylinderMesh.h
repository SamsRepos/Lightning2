#pragma once

#include "BaseMesh.h"

class CylinderMesh : public BaseMesh
{
public:
	CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius = .1f, float _resolution = 50);
	~CylinderMesh();
	
private:
	void initBuffers(ID3D11Device* device);
	float resolution;
	float radius;
};