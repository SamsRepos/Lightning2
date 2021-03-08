#pragma once

#include "BaseMesh.h"
#include <vector>

#include "Segment.h"
#include "MyLerp.h"
#include "MyClamp.h"

////
// Line
////

class Line {
public:
	Line(Segment* seg);
	
	inline XMFLOAT3 GetStart() { return start; };
	inline XMFLOAT3 GetEnd() { return end; };

	inline void SetVisible(bool _visible) { visible = _visible; };
	inline bool IsVisible() { return visible; };

	inline void SetParent(Line* _parent) { parent = _parent; };
	inline Line* GetParent() { return parent; };
	inline void AddChild(Line* child) { children.push_back(child); };
	inline std::vector<Line*>* GetChildren() { return &children; };
	inline Line* GetChild(size_t index) { return children[index]; };

	void InitAnimation();

	bool UpdateAnimationRecurs(float deltaTime);

private:
	XMFLOAT3 start;
	XMFLOAT3 end;
	XMFLOAT3 fixedEnd;
	
	float t; // for animation. 0 <= t <= 1
	float velocity;
	float length;
	bool visible;
	bool finishedAnimating;

	Line* parent;
	std::vector<Line*> children;
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

