#pragma once
#include "../SceneObject.h"

#include "../AnimSegment.h"

class CylinderObject : public SceneObject
{
public:
	CylinderObject(
		BaseMesh* _mesh,
		ID3D11ShaderResourceView* _texture,
		const XMMATRIX &_worldOriginTransform		
	);
		
	void Init(AnimSegment* _animSeg);

	inline void SetBrightness(float _brightness) { brightness = _brightness; };
	inline float GetBrightness() { return brightness; };

	inline bool IsVisible() { return animSeg->IsVisible(); };

	void UpdateFromAnimation();
	
protected:
	AnimSegment* animSeg;

	float brightness;
};

