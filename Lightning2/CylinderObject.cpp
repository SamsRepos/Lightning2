#include "CylinderObject.h"

CylinderObject::CylinderObject(
	BaseMesh* _mesh,
	ID3D11ShaderResourceView* _texture,
	const XMMATRIX &_worldOriginTransform
)
	:
	SceneObject::SceneObject(_mesh, _texture, _worldOriginTransform)
{
}

void CylinderObject::InitAnimation()
{
	t = 0;
	SetScale(t);
}

bool CylinderObject::UpdateAnimation(float deltaTime)
{
	float deltaLength = velocity * deltaTime;
	t += (deltaLength / length);

	SetScale(t);

	if (t >= 1.f)
	{
		SetScale(1.f); //ensuring scale doesnt't go too far
		return true;
	}

	return false;
}