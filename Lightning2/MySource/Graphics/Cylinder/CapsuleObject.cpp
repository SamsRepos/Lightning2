#include "CapsuleObject.h"

CapsuleObject::CapsuleObject(
	BaseMesh* _cylinderMesh,
	BaseMesh* _domeMesh,
	ID3D11ShaderResourceView* _texture,
	const XMMATRIX &_worldOriginTransform
)
	:
	CylinderObject::CylinderObject(_cylinderMesh, _texture, _worldOriginTransform)
{
}

void CapsuleObject::Init(AnimSegment* _animSeg){

	CylinderObject::Init(_animSeg);
}

void CapsuleObject::UpdateFromAnimation()
{
	CylinderObject::UpdateFromAnimation();

	SetScale(
		animSeg->GetDiameter(),
		animSeg->GetCurrentLength(),
		animSeg->GetDiameter()
	);
	BuildTransform();
}