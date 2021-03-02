#pragma once
#include "SceneObject.h"

class CylinderObject : public SceneObject
{
public:
	CylinderObject(
		BaseMesh* _mesh,
		ID3D11ShaderResourceView* _texture,
		const XMMATRIX &_worldOriginTransform
	);
	

	inline void SetBrightness(float _brightness) { brightness = _brightness; };
	inline float GetBrightness() { return brightness; };

private:
	float brightness;
};

