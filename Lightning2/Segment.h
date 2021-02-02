#pragma once

#include <vector>

#include "MyFloat3.h"

enum SegmentStatuses
{
	PRIMARY,
	SECONDARY	
};

class Segment
{	
public:
	Segment(
		MyFloat3 startPoint = MyFloat3(0.f, 0.f, 0.f),
		MyFloat3 endPoint   = MyFloat3(0.f, 0.f, 0.f),
		float diameter      = 1.f,
		float minDiameter   = 1.f
	);

	inline void SetStartPoint(MyFloat3 startPt) { startPoint = startPt; };
	inline MyFloat3 GetStartPoint() { return startPoint; };
	inline void SetEndPoint(MyFloat3 endPt) { endPoint = endPt; };
	inline MyFloat3 GetEndPoint() { return endPoint; };
	inline MyFloat3 GetDirection() { return endPoint - startPoint; };
	inline MyFloat3 GetMidpoint() { return startPoint + (GetDirection() * .5f); };
	inline MyFloat3 GetOffsetMidpoint(const float& t) { return startPoint + (GetDirection() * t); };
	inline void SetDiameter(float _diameter) { diameter = _diameter; };
	inline float GetDiameter() const { return diameter; };
	inline float GetMinDiameter() const { return minDiameter; };
	inline float GetLength() { return Magnitude(GetDirection()); };

	inline void SetParent(Segment* _parent) { parent = _parent; };
	inline Segment* GetParent() { return parent; };
	inline void AddChild(Segment* child) { children.push_back(child); };
	inline std::vector<Segment*>* GetChildren() { return &children; };
	inline Segment* GetChild(size_t index) { return children[index]; };

	inline void SetNumDescendants(size_t num) { numDescendants = num; };
	inline size_t GetNumDescendants() { return numDescendants; };

	inline void SetDistanceFromRoot(Segment* root) { distanceFromRoot = Magnitude(endPoint - root->GetStartPoint()); };
	inline float GetDistanceFromRoot() { return distanceFromRoot; };

	inline void SetFarthestDistanceOnThisPath(float dist) { farthestDistOnThisPath = dist; };
	inline float GetFarthestDistanceOnThisPath() { return farthestDistOnThisPath; }
	
	inline void SetStatus(SegmentStatuses _status) { status = _status; };
	inline SegmentStatuses GetStatus() { return status; };

	void SetToBeCulled();
	inline bool IsToBeCulled() { return cullMe; };
	
private:
	MyFloat3 startPoint, endPoint;
	float diameter, minDiameter;

	Segment* parent;
	std::vector<Segment*> children;

	size_t numDescendants;
	float distanceFromRoot;
	float farthestDistOnThisPath;
	SegmentStatuses status;

	bool cullMe = false;
};
