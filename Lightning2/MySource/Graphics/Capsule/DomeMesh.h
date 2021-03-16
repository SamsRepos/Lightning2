#pragma once

#include "BaseMesh.h"

class DomeMesh : public BaseMesh
{
public:
	DomeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float radius = .1f, float _resolution = 50);
	~DomeMesh();
	
private:
	void initBuffers(ID3D11Device* device);
	float resolution;
	float radius;
};