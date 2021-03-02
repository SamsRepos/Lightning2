#include "SceneObject.h"

SceneObject::SceneObject(BaseMesh* _mesh, ID3D11ShaderResourceView* _texture, const XMMATRIX &_worldOriginTransform)
	:
	mesh(_mesh),
	texture(_texture),
	worldOriginTransform(_worldOriginTransform)
{
	//defaults:
	SetPosition(0.f, 0.f, 0.f);
	SetScale(1.f, 1.f, 1.f);
	SetRotation(0.f, 0.f, 0.f);
	BuildTransform();
}

void SceneObject::BuildTransform() 
{
	transform = worldOriginTransform;
	
	//translate:
	XMMATRIX translateMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	transform = XMMatrixMultiply(translateMatrix, transform);

	//rotation:
	//XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw((rotation.x), (rotation.y), (rotation.z));
	transform = XMMatrixMultiply(rotationMatrix, transform);
	
	//scale:
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	transform = XMMatrixMultiply(scaleMatrix, transform);
};
