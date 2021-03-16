#pragma once

#include "MyBaseShader.h"

class CapsuleShader : public MyBaseShader
{
private:
	struct ColourBufferType {
		XMFLOAT4 colour;
	};
public:
	CapsuleShader(ID3D11Device* device, HWND hwnd);
	~CapsuleShader();

	void SetShaderParameters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX &world,
		const XMMATRIX &view,
		const XMMATRIX &projection,
		ID3D11ShaderResourceView* texture		
	);

	void SetColour(
		ID3D11DeviceContext* deviceContext,
		XMFLOAT4 colour
	);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* colourBuffer;
};

