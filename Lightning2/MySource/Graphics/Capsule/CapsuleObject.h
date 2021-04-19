#pragma once
#include "../SceneObject.h"

#include "../AnimSegment.h"
#include "Shaders/MyBaseShader.h"

class CapsuleObject : public SceneObject
{
public:
	CapsuleObject(
		BaseMesh* _cylinderMesh,
		BaseMesh* _domeMesh,
		ID3D11ShaderResourceView* _texture,
		const XMMATRIX &_worldOriginTransform		
	);
		
	void Init(AnimSegment* _animSeg);
	
	inline void SetBrightness(float _brightness) { brightness = _brightness; };
	inline float GetBrightness() { return brightness; };

	inline bool IsVisible() { return animSeg->IsVisible(); };

	// Updates transforms to full size, for static rendering
	void InitStatic();

	// Uses associated AnimSegment data to update transforms
	void UpdateFromAnimation();

	void Render(
		MyBaseShader* shader,
		D3D* renderer,
		const XMMATRIX& viewMatrix,
		const XMMATRIX& projectionMatrix,
		const XMFLOAT4& colour
	);

	//inline XMMATRIX GetDomeTransform1() { return domeTransform1; };
	//inline XMMATRIX GetDomeTransform2() { return domeTransform2; };
	//inline BaseMesh* GetDomeMesh() { return domeMesh; };
	
private:
	void BuildAllTransforms();
	void BuildDomeTransforms();
	
	AnimSegment* animSeg;

	float brightness;

	BaseMesh* cylinderMesh;
	BaseMesh* domeMesh;
	
	XMMATRIX cylinderTransform;
	XMMATRIX domeTransform1;
	XMMATRIX domeTransform2;
};

