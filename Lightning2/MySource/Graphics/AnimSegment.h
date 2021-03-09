#pragma once

#include <vector>

#include "Segment.h"

class AnimSegment
{	
public:
	AnimSegment(Segment* seg);

	inline void SetVisible(bool _visible) { visible = _visible; };
	inline bool IsVisible() { return visible; };
	
	inline MyFloat3 GetStartPoint() { return startPoint; };
	inline MyFloat3 GetCurrentEndPoint() { return currentEndPoint; };
	inline MyFloat3 GetFixedEndPoint() { return fixedEndPoint; };
	
	inline float GetCurrentLength() { return currentLength; };
	inline float GetFixedLength() { return fixedLength; };
	
	inline float GetDiameter() const { return diameter; };

	inline void SetParent(AnimSegment* _parent) { parent = _parent; };
	inline AnimSegment* GetParent() { return parent; };
	inline void AddChild(AnimSegment* child) { children.push_back(child); };
	inline std::vector<AnimSegment*>* GetChildren() { return &children; };
	inline AnimSegment* GetChild(size_t index) { return children[index]; };

	inline SegmentStatuses GetStatus() { return status; };
	inline float GetEnergy() { return energy; };
	inline float GetVelocity() { return velocity; };

	void InitAnimation();

	bool UpdateAnimationRecurs(float dt);

private:
	bool visible;
	bool animatingNow;

	MyFloat3 startPoint;
	MyFloat3 currentEndPoint;
	MyFloat3 fixedEndPoint;
	float currentLength;
	float fixedLength;
	MyFloat3 direction;

	float diameter;
	float velocity;
	float energy;
	SegmentStatuses status;

	AnimSegment* parent;
	std::vector<AnimSegment*> children;
};
