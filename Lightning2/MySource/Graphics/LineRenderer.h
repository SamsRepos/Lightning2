#pragma once

#include "Shaders/LineShader.h"
#include "Graphics/LineMesh.h"
#include "Segment.h"

class LineRenderer
{
public:
	LineRenderer(D3D* renderer, HWND hwnd);
	~LineRenderer();
	
	inline void SetColour(const XMFLOAT4& _lineColour) { lineColour = _lineColour; };

	void Build(std::vector<Segment*>* segments);
	
	void InitAnimation();
	bool UpdateAnimation(float dt); //returns true when animation is over
	
	void SetShaderParams(const XMMATRIX& _worldMatrix, const XMMATRIX& _viewMatrix, const XMMATRIX& _projectionMatrix);
	void RenderLines(D3D* renderer);
	
private:
	void CreateLinesRecurs(Segment* seg, Line* parentLine);

	std::vector<Line*> lines;

	LineShader* shader;
	LineMesh* mesh;
	
	XMFLOAT4 lineColour;

	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;

	bool animatingNow;
};

