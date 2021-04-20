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
	
	void SetColours(
		const XMFLOAT4& _backgroundColour,
		const XMFLOAT4& _lineColour
	);

	void Build(std::vector<AnimSegment*>* animSegs, EnergyScales energyScale);
			
	void SetShaderParams(const XMMATRIX& _worldMatrix, const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderLines(D3D* renderer, LightningRenderModes renderMode, bool energyForBrightness);
	
	void ClearLines();

private:
	
	std::vector<LineMesh*> lines;

	LineShader* shader;

	D3D* renderer;
		
	XMFLOAT4 backgroundColour;
	XMFLOAT4 lineColour;
	
	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	bool animatingNow;
};

