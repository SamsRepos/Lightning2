#pragma once

#include "DXF.h"

class SceneObject
{
protected:
	BaseMesh *mesh;
	ID3D11ShaderResourceView* texture;

	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;

	XMMATRIX worldMatrix;
	XMMATRIX objectMatrix;
	
public:
	SceneObject(BaseMesh* _mesh, ID3D11ShaderResourceView* _texture, const XMMATRIX &_worldMatrix);
	~SceneObject();

	BaseMesh* getMesh() { return mesh; };
	ID3D11ShaderResourceView* getTexture() { return texture; };

	//transform stuff:
	void setPosition(XMFLOAT3 pos) { position = pos; };
	void setPosition(float x, float y, float z) { position = XMFLOAT3(x, y, z); };
	XMFLOAT3 getPosition() { return position; };
	void setScale (XMFLOAT3 sca) { scale = sca; };
	void setScale(float x, float y, float z) { scale = XMFLOAT3(x, y, z); };
	void setScale(float sca) { scale = XMFLOAT3(sca, sca, sca); };
	XMFLOAT3 getScale() { return scale; };
	void setRotation(XMFLOAT3 rot) { rotation = rot; };
	void setRotation(float x, float y, float z) { rotation = XMFLOAT3(x, y, z); };
	XMFLOAT3 getRotation() { return rotation; };

	void setWorldMatrix(XMMATRIX _worldMatrix) { worldMatrix = _worldMatrix; };
	XMMATRIX getWorldMatrix() { return worldMatrix; };
	
	void updateObjectMatrix();
	XMMATRIX getObjectMatrix() { return objectMatrix; };
		
};

