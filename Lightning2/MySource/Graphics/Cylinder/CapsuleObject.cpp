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
	domeMesh = _domeMesh;
}

void CapsuleObject::Init(AnimSegment* _animSeg){

	CylinderObject::Init(_animSeg);
}

void CapsuleObject::UpdateFromAnimation()
{
	CylinderObject::UpdateFromAnimation();

	BuildDomeTransforms();
}

void CapsuleObject::BuildDomeTransforms()
{
	//TRANSLATION:
	//Dome 1 is at the start of the cylinder::
	XMMATRIX translateMatrix1 = XMMatrixTranslation(position.x, position.y, position.z);
	domeTransform1 = XMMatrixMultiply(translateMatrix1, worldOriginTransform);
	//Dome 2 is at the end of the cylinder:
	XMMATRIX translateMatrix2 = XMMatrixTranslation(
		animSeg->GetCurrentEndPoint().x,
		animSeg->GetCurrentEndPoint().y,
		animSeg->GetCurrentEndPoint().z
	);
	domeTransform2 = XMMatrixMultiply(translateMatrix2, worldOriginTransform);

	//ROTATION:
	XMMATRIX cylinderRotationMatrix = XMMatrixRotationRollPitchYaw((rotation.x), (rotation.y), (rotation.z));
	domeTransform1 = XMMatrixMultiply(cylinderRotationMatrix, domeTransform1);
	domeTransform2 = XMMatrixMultiply(cylinderRotationMatrix, domeTransform2);

	//scale:
	float domeScale = animSeg->GetDiameter();

	//for the top dome, flipping the Y direction to invert dome obviously
	//and flipping one other direction, so that polygons face outwards again
	XMMATRIX scaleMatrix1 = XMMatrixScaling(-domeScale, -domeScale, domeScale);
	domeTransform1 = XMMatrixMultiply(scaleMatrix1, domeTransform1);

	XMMATRIX scaleMatrix2 = XMMatrixScaling(domeScale, domeScale, domeScale);
	domeTransform2 = XMMatrixMultiply(scaleMatrix2, domeTransform2);
};