#include "BlurShader.h"

BlurShader::BlurShader(ID3D11Device* device, HWND hwnd) : BaseShader(device, hwnd)
{
	initShader(L"blur_vs.cso", L"blur_ps.cso");
}


BlurShader::~BlurShader()
{
	// Release the sampler state.
	if (sampleState)
	{
		sampleState->Release();
		sampleState = 0;
	}

	// Release the matrix constant buffer.
	if (matrixBuffer)
	{
		matrixBuffer->Release();
		matrixBuffer = 0;
	}
	

	BaseShader::~BaseShader();
}

void BlurShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	//matrix buffer:
	D3D11_BUFFER_DESC matrixBufferDesc;
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	//sampler:
	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);

	//screen size:
	D3D11_BUFFER_DESC screenSizeBufferDesc;
	screenSizeBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	screenSizeBufferDesc.ByteWidth = sizeof(ScreenSizeBufferType);
	screenSizeBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	screenSizeBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	screenSizeBufferDesc.MiscFlags = 0;
	screenSizeBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&screenSizeBufferDesc, NULL, &screenSizeBuffer);

	//blur info:
	//directional:
	{
		D3D11_BUFFER_DESC d;
		d.Usage = D3D11_USAGE_DYNAMIC;
		d.ByteWidth = sizeof(DirectionalBufferType);
		d.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d.MiscFlags = 0;
		d.StructureByteStride = 0;
		renderer->CreateBuffer(&d, NULL, &directionalBuffer);
	}

	//motion:
	{
		D3D11_BUFFER_DESC d;
		d.Usage = D3D11_USAGE_DYNAMIC;
		d.ByteWidth = sizeof(MotionBufferType);
		d.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d.MiscFlags = 0;
		d.StructureByteStride = 0;
		renderer->CreateBuffer(&d, NULL, &motionBuffer);
	}

}

void BlurShader::setScreenSize(ID3D11DeviceContext* deviceContext, XMINT2 size) {

	ScreenSizeBufferType* sPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->Map(screenSizeBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	sPtr = (ScreenSizeBufferType*)mappedResource.pData;

	sPtr->screenSize.x = float(size.x);
	sPtr->screenSize.y = float(size.y);

	deviceContext->Unmap(screenSizeBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &screenSizeBuffer);

}

void BlurShader::updateDirectionalBlurParameters(ID3D11DeviceContext* deviceContext, float blurExtent, float blurTheta) {
	
	DirectionalBufferType* dPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->Map(directionalBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dPtr = (DirectionalBufferType*)mappedResource.pData;

	dPtr->extent = blurExtent;
	dPtr->theta = blurTheta;

	deviceContext->Unmap(directionalBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &directionalBuffer);

}

void BlurShader::updateMotionBlurParameters(ID3D11DeviceContext* deviceContext, float blurExtent) {
	
	MotionBufferType* mPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->Map(motionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	mPtr = (MotionBufferType*)mappedResource.pData;

	mPtr->extent = blurExtent;

	deviceContext->Unmap(motionBuffer, 0);
	deviceContext->PSSetConstantBuffers(2, 1, &motionBuffer);

}

void BlurShader::updateMotionBlurTextures(ID3D11DeviceContext* deviceContext, RenderTexture** textures, int texturesNum) {

	for (size_t i = 0; i < texturesNum; i++) {
		ID3D11ShaderResourceView* texture = textures[i]->getShaderResourceView();
		deviceContext->PSSetShaderResources((i + 1), 1, &texture);
	}
}

void BlurShader::setShaderParameters(ID3D11DeviceContext* deviceContext, const XMMATRIX &world, const XMMATRIX &view, const XMMATRIX &projection, ID3D11ShaderResourceView* texture) {

	D3D11_MAPPED_SUBRESOURCE mappedResource;

	//matrices:
	XMMATRIX tWorld = XMMatrixTranspose(world);
	XMMATRIX tView = XMMatrixTranspose(view);
	XMMATRIX tProj = XMMatrixTranspose(projection);

	MatrixBufferType* mPtr;

	deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	mPtr = (MatrixBufferType*)mappedResource.pData;

	mPtr->world = tWorld;
	mPtr->view = tView;
	mPtr->projection = tProj;

	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	//texture and sampler:
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);

}