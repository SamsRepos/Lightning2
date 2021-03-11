#pragma once
#include "CylinderObject.h"

class CapsuleObject : public CylinderObject
{
public:
	CapsuleObject(
		BaseMesh* _cylinderMesh,
		BaseMesh* _domeMesh,
		ID3D11ShaderResourceView* _texture,
		const XMMATRIX &_worldOriginTransform		
	);
		
	void Init(AnimSegment* _animSeg);
	
	void UpdateFromAnimation();

	inline XMMATRIX GetDomeTransform1() { return domeTransform1; };
	inline XMMATRIX GetDomeTransform2() { return domeTransform2; };
	inline BaseMesh* GetDomeMesh() { return domeMesh; };
	
private:
	void BuildDomeTransforms();

	BaseMesh* domeMesh;
	
	XMMATRIX domeTransform1;
	XMMATRIX domeTransform2;
};

