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
	{
		D3D11_BUFFER_DESC d;
		d.Usage = D3D11_USAGE_DYNAMIC;
		d.ByteWidth = sizeof(BlurBufferType);
		d.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		d.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		d.MiscFlags = 0;
		d.StructureByteStride = 0;
		renderer->CreateBuffer(&d, NULL, &blurBuffer);
	}

	// energy map sampler:
	{
		D3D11_SAMPLER_DESC d;
		d.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		d.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		d.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		d.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		d.BorderColor[0] = 1.0f;
		d.BorderColor[1] = 1.0f;
		d.BorderColor[2] = 1.0f;
		d.BorderColor[3] = 1.0f;

		renderer->CreateSamplerState(&d, &energySampleState);
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

void BlurShader::updateBlurParameters(
	ID3D11DeviceContext* deviceContext,
	RenderTexture* energyTexture,
	float _directions,
	float _quality,
	float _size,
	float _finalAdjustment
)
{	
	BlurBufferType* blurPtr;
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	deviceContext->Map(blurBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	blurPtr = (BlurBufferType*)mappedResource.pData;

	blurPtr->directions      = _directions;
	blurPtr->quality         = _quality;
	blurPtr->size            = _size;
	blurPtr->finalAdjustment = _finalAdjustment;

	deviceContext->Unmap(blurBuffer, 0);
	deviceContext->PSSetConstantBuffers(1, 1, &blurBuffer);

	ID3D11ShaderResourceView* energyMap = energyTexture->getShaderResourceView();
	deviceContext->PSSetShaderResources(1, 1, &energyMap);
	deviceContext->PSSetSamplers(1, 1, &energySampleState);

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