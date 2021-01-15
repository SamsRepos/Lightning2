#pragma once
#include "BaseMesh.h"
#include <vector>

class Line {
public:
	Line() {
		start = XMFLOAT3(0, 0, 0); end = XMFLOAT3(1, 1, 1);
	}
	Line(XMFLOAT3 s, XMFLOAT3 e, int l) {
		start = s;
		end = e;
		level = l;
	}
	
	int level;
	XMFLOAT3 start;
	XMFLOAT3 end;
};

class LineMesh :
	public BaseMesh
{
public:
	
	LineMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext);
	~LineMesh();

	void	sendData(ID3D11DeviceContext* deviceContext, int line, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_LINELIST);

	size_t GetLineCount() { return lines.size(); }

	Line* getLine(size_t i) { return &(lines[i]); };

	size_t getMaxLevel() { return maxLevel; };

	//Line manipulation
	void	AddLine(XMFLOAT3 start, XMFLOAT3 end, int level);
	void	Clear() { lines.clear(); maxLevel = 0; }

private:
	VertexType*			vertices;
	unsigned long*		indices;
	std::vector<Line>	lines;
	
	size_t maxLevel;
	
	void	initBuffers(ID3D11Device* device);
	void	LoadLine(ID3D11DeviceContext* deviceContext, int lineNo);
};

