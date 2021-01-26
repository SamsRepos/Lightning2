#include "SceneObject.h"

SceneObject::SceneObject(BaseMesh* _mesh, ID3D11ShaderResourceView* _texture, const XMMATRIX &_worldMatrix)
	:
	mesh(_mesh),
	texture(_texture),
	worldMatrix(_worldMatrix)
{
	//defaults:
	setPosition(0.f, 0.f, 0.f);
	setScale(1.f, 1.f, 1.f);
	setRotation(0.f, 0.f, 0.f);
	updateObjectMatrix();
}

SceneObject::~SceneObject()
{
}

void SceneObject::updateObjectMatrix() 
{
	objectMatrix = worldMatrix;
	
	//translate:
	XMMATRIX translateMatrix = XMMatrixTranslation(position.x, position.y, position.z);
	objectMatrix = XMMatrixMultiply(translateMatrix, objectMatrix);

	//rotation:
	//XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw(XMConvertToRadians(rotation.x), XMConvertToRadians(rotation.y), XMConvertToRadians(rotation.z));
	XMMATRIX rotationMatrix = XMMatrixRotationRollPitchYaw((rotation.x), (rotation.y), (rotation.z));
	objectMatrix = XMMatrixMultiply(rotationMatrix, objectMatrix);
	
	//scale:
	XMMATRIX scaleMatrix = XMMatrixScaling(scale.x, scale.y, scale.z);
	objectMatrix = XMMatrixMultiply(scaleMatrix, objectMatrix);	
};
