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
	float diameter;
	float fixedLength;
	float currentLength;
	float velocity;
	float brightness;
	
	bool visible;
	bool finishedAnimating;
	
	CylinderObject* parent;
	std::vector<CylinderObject*> children;
};

