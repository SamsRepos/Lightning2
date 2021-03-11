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

	XMMATRIX GetDomeTransform1();
	XMMATRIX GetDomeTransform2();
	inline BaseMesh* GetDomeMesh() { return domeMesh; };
	
private:
	BaseMesh* domeMesh;
};

