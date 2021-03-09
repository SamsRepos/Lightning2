#pragma once

#include "BaseMesh.h"
#include <vector>

#include "AnimSegment.h"
#include "Maths/MyLerp.h"
#include "Maths/MyClamp.h"

////
// Line
////

class Line {
public:
	Line(AnimSegment* _animSeg);
	
	XMFLOAT3 GetStart();
	XMFLOAT3 GetCurrentEnd();
	XMFLOAT3 GetFixedEnd();
	
private:
	AnimSegment* animSeg;
};

////
// LineMesh
////

class LineMesh :
	public BaseMesh
{
public:
	
	LineMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~LineMesh();

	void sendData(ID3D11DeviceContext* deviceContext, int line, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_LINELIST);
					
	//Line manipulation
	inline void SetLines(std::vector<Line*>* _lines) { lines = _lines; };
	int GetLineCount();
	
private:
	VertexType*			vertices;
	unsigned long*		indices;
	std::vector<Line*>*	lines;
		
	void	initBuffers(ID3D11Device* device);
	void	LoadLine(ID3D11DeviceContext* deviceContext, int lineNo);
};

