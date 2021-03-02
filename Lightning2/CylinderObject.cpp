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




