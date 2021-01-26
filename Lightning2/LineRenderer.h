#pragma once

#include "LineShader.h"
#include "LineMesh.h"
#include "Segment.h"

class LineRenderer
{
public:
	LineRenderer();
	~LineRenderer();
	
	void Init(D3D* renderer, HWND hwnd);	
	void Build(std::vector<Segment*>* segments);
	void SetShaderParams(
		const XMMATRIX& _worldMatrix, 
		const XMMATRIX& _viewMatrix, 
		const XMMATRIX& _projectionMatrix, 
		Light* _light, 
		const XMFLOAT4& _colour
	);
	void Render(D3D* renderer);
	
private:
	LineShader* shader;
	LineMesh* mesh;

	XMMATRIX worldMatrix;
	XMMATRIX viewMatrix;
	XMMATRIX projectionMatrix;
	Light* light;
	XMFLOAT4 colour;

	//debug stuff:
	int linesToRender;
};

