#pragma once
#include "SceneObject.h"

class CylinderObject : public SceneObject
{
public:
	CylinderObject(
		BaseMesh* _mesh,
		ID3D11ShaderResourceView* _texture,
		const XMMATRIX &_worldOriginTransform
	);
	
	inline void SetBrightness(float _brightness) { brightness = _brightness; };
	inline float GetBrightness() { return brightness; };

	inline void SetLength(float _length) { length = _length; };
	inline float GetLength() { return length; };

	inline void SetVelocity(float _velocity) { velocity = _velocity; };
	inline float GetVelocity() { return velocity; };

	inline void SetParent(CylinderObject* _parent) { parent = _parent; };
	inline CylinderObject* GetParent() { return parent; };
	inline void AddChild(CylinderObject* child) { children.push_back(child); };
	inline std::vector<CylinderObject*>* GetChildren() { return &children; };
	inline CylinderObject* GetChild(size_t index) { return children[index]; };

	void InitAnimation();
	bool UpdateAnimation(float deltaTime); //Returns true if animation is completed

private:
	float brightness;
	float length;
	float velocity;
	float t; // for animation.. 0 <= t <= 1

	CylinderObject* parent;
	std::vector<CylinderObject*> children;
};

