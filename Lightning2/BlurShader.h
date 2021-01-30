#pragma once

#include "DXF.h"

using namespace DirectX;

#define MAX_MOTION_TEXTURES_NUM 4

class BlurShader : public BaseShader
{

	struct ScreenSizeBufferType {
		XMFLOAT2 screenSize;
		XMFLOAT2 padding;
	};

	//blur info buffer types:
	struct DirectionalBufferType {
		float extent;
		float theta;
		XMFLOAT2 padding;
	};

	struct MotionBufferType {
		float extent;
		XMFLOAT3 padding;
	};

	
public:
	BlurShader(ID3D11Device* device, HWND hwnd);
	~BlurShader();


	void setScreenSize(ID3D11DeviceContext* deviceContext, XMINT2 size);
	void updateDirectionalBlurParameters(ID3D11DeviceContext* deviceContext, float blurExtent, float blurTheta);
	void updateMotionBlurParameters(ID3D11DeviceContext* deviceContext, float blurExtent);
	void updateMotionBlurTextures(ID3D11DeviceContext* deviceContext, RenderTexture** textures, int texturesNum);

	void setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture);
private:
	void initShader(const wchar_t* vs, const wchar_t* ps);
	
	ID3D11Buffer * matrixBuffer;
	ID3D11SamplerState* sampleState;
	
	ID3D11Buffer* screenSizeBuffer;
	
	ID3D11Buffer* directionalBuffer;
	ID3D11Buffer* motionBuffer;
	
};

