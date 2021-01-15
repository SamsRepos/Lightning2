#pragma once

//#include "DXF.h"
#include "DXF.h"

using namespace std;
using namespace DirectX;

class LineShader : public BaseShader
{
private:
	struct ColourBufferType {
		XMFLOAT4 colour;
	};

public:
	LineShader(ID3D11Device* device, HWND hwnd);
	~LineShader();

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture, Light* light, XMFLOAT4 colour);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;

	ID3D11Buffer* colourBuffer;
};

