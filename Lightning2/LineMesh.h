#pragma once
#include "BaseMesh.h"
#include <vector>

//for Line class only:
#include "Segment.h"
#include "MyLerp.h"
#include "MyClamp.h"

class Line {
public:
	Line(Segment* seg)
	{
		start = XMFLOAT3(
			seg->GetStartPoint().x,
			seg->GetStartPoint().y,
			seg->GetStartPoint().z
		);
				
		end = XMFLOAT3(
			seg->GetEndPoint().x,
			seg->GetEndPoint().y,
			seg->GetEndPoint().z
		);

		fixedEnd = end;
		velocity = seg->GetVelocity();
		length = seg->GetLength();
	}
	
	inline XMFLOAT3 GetStart() { return start; };
	inline XMFLOAT3 GetEnd() { return end; };

	inline void SetVisible(bool _visible) { visible = _visible; };
	inline bool IsVisible() { return visible; };

	inline void SetParent(Line* _parent) { parent = _parent; };
	inline Line* GetParent() { return parent; };
	inline void AddChild(Line* child) { children.push_back(child); };
	inline std::vector<Line*>* GetChildren() { return &children; };
	inline Line* GetChild(size_t index) { return children[index]; };

	void InitAnimation()
	{
		t = 0.f;
		end = start;
		finishedAnimating = false;
		SetVisible(false);
	}

	bool UpdateAnimationRecurs(float deltaTime)
	{
		if (finishedAnimating)
		{
			bool res = false;

			for (Line* child : children)
			{
				res = child->UpdateAnimationRecurs(deltaTime) && res;
			}
			return res;
		}

		// Growth:
		float deltaLength = velocity * deltaTime;
		t += deltaLength / length;
		t = MyClamp(t, 0.f, 1.f);

		end = XMFLOAT3(
			MyLerp(start.x, fixedEnd.x, t),
			MyLerp(start.y, fixedEnd.y, t),
			MyLerp(start.z, fixedEnd.z, t)
		);

		if (t >= 1.f)
		{
			finishedAnimating = true;
			end = fixedEnd; // ensuring any slight t>1.f is fixed

			if (children.empty())
			{
				return true;
			}
			else
			{
				for (Line* child : children)
				{
					child->SetVisible(true);
				}
			}
		}

		return false;
	}

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

