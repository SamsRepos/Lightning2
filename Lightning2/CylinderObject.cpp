#include "CylinderObject.h"

#include "MyClamp.h"
#include "MyLerp.h"

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
	fullScale = GetScale();

	t = 0;
	SetScale(0);
	BuildTransform();
}

bool CylinderObject::UpdateAnimation(float deltaTime)
{
	float deltaLength = velocity * deltaTime;
	t += (deltaLength / length);
	t = MyClamp(t, 0.f, 1.f);

	SetScale(
		XMFLOAT3(
			MyLerp(0.f, fullScale.x, t),
			MyLerp(0.f, fullScale.y, t),
			MyLerp(0.f, fullScale.z, t)
		)
	);


	BuildTransform();
	
	if (t >= 1.f)
	{		
		return true;
	}

	return false;
}