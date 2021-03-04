#pragma once
#include "SceneObject.h"

#include "Segment.h"

class CylinderObject : public SceneObject
{
public:
	CylinderObject(
		BaseMesh* _mesh,
		ID3D11ShaderResourceView* _texture,
		const XMMATRIX &_worldOriginTransform
	);

	void Init(Segment* seg);
	
	inline void SetBrightness(float _brightness) { brightness = _brightness; };
	inline float GetBrightness() { return brightness; };

	inline void SetLength(float _length) { length = _length; };
	inline float GetLength() { return length; };

	inline void SetVelocity(float _velocity) { velocity = _velocity; };
	inline float GetVelocity() { return velocity; };

	inline void SetVisible(bool _visible) { visible = _visible; };
	inline bool IsVisible() { return visible; };

	inline void SetParent(CylinderObject* _parent) { parent = _parent; };
	inline CylinderObject* GetParent() { return parent; };
	inline void AddChild(CylinderObject* child) { children.push_back(child); };
	inline std::vector<CylinderObject*>* GetChildren() { return &children; };
	inline CylinderObject* GetChild(size_t index) { return children[index]; };

	void InitAnimation();
	bool UpdateAnimationRecurs(float deltaTime); //Returns true if animation is completed
	
private:
	float brightness;
	float length;
	float velocity;
	float t; // for animation.. 0 <= t <= 1
	bool visible;
	bool finishedAnimating;

	XMFLOAT3 fullScale;

	CylinderObject* parent;
	std::vector<CylinderObject*> children;
};

