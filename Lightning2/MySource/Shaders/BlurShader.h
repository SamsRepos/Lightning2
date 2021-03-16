#pragma once

#include "MyBaseShader.h"

class BlurShader : public MyBaseShader
{
	struct ScreenSizeBufferType {
		XMFLOAT2 screenSize;
		XMFLOAT2 padding;
	};

	//blur info buffer types:
	struct BlurBufferType {
		float directions;
		float quality;
		float size;
		float finalAdjustment;
	};	
	
public:
	BlurShader(ID3D11Device* device, HWND hwnd);
	~BlurShader();
	
	void SetScreenSize(ID3D11DeviceContext* deviceContext, XMINT2 size);
	
	void SetBlurParameters(
		ID3D11DeviceContext* deviceContext, 
		RenderTexture* energyTexture,
		float _directions = 16.f, 
		float _quality = 30.f, 
		float _size = 9.f, 
		float _finalAdjustment = 15.f);
	
	void SetShaderParameters(
		ID3D11DeviceContext* deviceContext,
		const XMMATRIX &world,
		const XMMATRIX &view,
		const XMMATRIX &projection,
		ID3D11ShaderResourceView* texture
	);

private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	
	ID3D11Buffer* matrixBuffer;
	ID3D11SamplerState* sampleState;
	
	ID3D11Buffer* screenSizeBuffer;	
	ID3D11Buffer* blurBuffer;

	ID3D11SamplerState* energySampleState;
};

