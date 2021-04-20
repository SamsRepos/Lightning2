#pragma once

#include "BaseMesh.h"
#include <vector>

#include "../AnimSegment.h"
#include "../RenderSettings.h"
#include "Maths/MyLerp.h"
#include "Maths/MyClamp.h"

//NB. This class was built on code provided in CMP305

class LineMesh :
	public BaseMesh
{
public:
	
	LineMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, AnimSegment* _animSeg);
	~LineMesh();

	inline void SetBrightness(float _brightness) { brightness = _brightness; };
	inline float GetBrightness() { return brightness; };

	void sendData(LightningRenderModes renderMode, ID3D11DeviceContext* deviceContext, int line, D3D_PRIMITIVE_TOPOLOGY top = D3D_PRIMITIVE_TOPOLOGY_LINELIST);
		
private:
	XMFLOAT3 GetStart();
	XMFLOAT3 GetEnd(LightningRenderModes renderMode);

	void	initBuffers(ID3D11Device* device);
	void	LoadLine(ID3D11DeviceContext* deviceContext, int lineNo, LightningRenderModes renderMode);

	VertexType*			vertices;
	unsigned long*		indices;
		
	AnimSegment* animSeg;

	float brightness;
};

