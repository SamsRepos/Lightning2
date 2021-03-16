#pragma once

//#include "DXF.h"
#include "DXF.h"

using namespace std;
using namespace DirectX;

class MyBaseShader : public BaseShader
{	
public:
	MyBaseShader(ID3D11Device* device, HWND hwnd);
	~MyBaseShader();

	virtual void SetShaderParameters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX &world,
		const XMMATRIX &view,
		const XMMATRIX &projection,
		ID3D11ShaderResourceView* texture
	) = 0;

};

