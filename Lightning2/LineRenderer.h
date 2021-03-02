#pragma once

#include "LineShader.h"
#include "LineMesh.h"
#include "Segment.h"

class LineRenderer
{
public:
	LineRenderer(D3D* renderer, HWND hwnd);
	~LineRenderer();
	
	void InitParameters(
		const XMFLOAT4& _lineColour
	);
	void Build(std::vector<Segment*>* segments);
	void SetShaderParams(const XMMATRIX& _worldMatrix, const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderLines(D3D* renderer);
	
private:
	LineShader* shader;
	LineMesh* mesh;

	XMFLOAT4 lineColour;

	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
};

