#pragma once

#include "LineMesh.h"
#include "../AnimSegment.h"
#include "../RenderSettings.h"
#include "Shaders/LineShader.h"

class LineRenderer
{
public:
	LineRenderer(D3D* renderer, HWND hwnd);
	~LineRenderer();
	
	inline void SetColour(const XMFLOAT4& _lineColour) { lineColour = _lineColour; };

	void Build(std::vector<AnimSegment*>* animSegs);
	
	void SetShaderParams(const XMMATRIX& _worldMatrix, const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderLines(D3D* renderer, LightningRenderModes renderMode);
	
	void ClearLines();

private:
	
	std::vector<Line*> lines;

	LineShader* shader;
	LineMesh* mesh;
	
	XMFLOAT4 lineColour;

	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	bool animatingNow;
};

