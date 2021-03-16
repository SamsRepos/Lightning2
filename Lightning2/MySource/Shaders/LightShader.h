#pragma once

#include "MyBaseShader.h"

class LightShader : public MyBaseShader
{
private:
	struct LightBufferType
	{
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
		XMFLOAT3 direction;
		float padding;
	};

public:
	LightShader(ID3D11Device* device, HWND hwnd);
	~LightShader();

	void SetShaderParameters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX &world,
		const XMMATRIX &view,
		const XMMATRIX &projection,
		ID3D11ShaderResourceView* texture
	);

	void SetLight(
		ID3D11DeviceContext* deviceContext,
		Light* light
	);

private:
	void initShader(const wchar_t* cs, const wchar_t* ps);

private:
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	ID3D11Buffer* lightBuffer;
};

