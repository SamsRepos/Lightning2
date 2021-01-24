#pragma once

#include "BaseMesh.h"

class CylinderMesh : public BaseMesh
{
	CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float _bottomRadius, float _topRadius, float _resolution);
	//CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float _botRadius = .1f, float _topRadius = .1f, float _resolution = 50);

private:
	void InitBuffers(ID3D11Device* device);
	float resolution;
	float bottomRadius, topRadius;
};