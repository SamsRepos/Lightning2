#pragma once

#include "DXF.h"

class SceneObject
{
public:
	SceneObject(BaseMesh* _mesh, ID3D11ShaderResourceView* _texture, const XMMATRIX &_worldOriginTransform);
	
	BaseMesh* GetMesh() { return mesh; };
	ID3D11ShaderResourceView* GetTexture() { return texture; };

	//transform stuff:
	inline void SetPosition(XMFLOAT3 pos) { position = pos; };
	inline void SetPosition(float x, float y, float z) { position = XMFLOAT3(x, y, z); };
	inline XMFLOAT3 GetPosition() { return position; };

	inline void SetScale (XMFLOAT3 sca) { scale = sca; };
	inline void SetScale(float x, float y, float z) { scale = XMFLOAT3(x, y, z); };
	inline void SetScale(float sca) { scale = XMFLOAT3(sca, sca, sca); };
	inline XMFLOAT3 GetScale() { return scale; };

	inline void SetRotation(XMFLOAT3 rot) { rotation = rot; };
	inline void SetRotation(float x, float y, float z) { rotation = XMFLOAT3(x, y, z); };
	inline XMFLOAT3 GetRotation() { return rotation; };

	inline void SetWorldOriginTransform(XMMATRIX _worldOriginTransform) { worldOriginTransform = _worldOriginTransform; };
	inline XMMATRIX GetWorldOriginTransform() { return worldOriginTransform; };
	
	void BuildTransform();
	inline XMMATRIX GetTransform() { return transform; };

protected:
	BaseMesh *mesh;
	ID3D11ShaderResourceView* texture;

	XMFLOAT3 position;
	XMFLOAT3 scale;
	XMFLOAT3 rotation;
	XMMATRIX worldOriginTransform;

	XMMATRIX transform;
};

