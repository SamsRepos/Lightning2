#include "LineShader.h"

LineShader::LineShader(ID3D11Device* device, HWND hwnd) 
	:
	MyBaseShader(device, hwnd)
{
	initShader(L"line_vs.cso", L"line_ps.cso");
}


LineShader::~LineShader()
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

	// Release the layout.
	if (layout)
	{
		layout->Release();
		layout = 0;
	}

	// Release the light constant buffer.
	if (colourBuffer)
	{
		colourBuffer->Release();
		colourBuffer = 0;
	}

	//Release base shader components
	MyBaseShader::~MyBaseShader();
}

void LineShader::initShader(const wchar_t* vsFilename, const wchar_t* psFilename)
{
	D3D11_BUFFER_DESC matrixBufferDesc;
	D3D11_SAMPLER_DESC samplerDesc;
	
	// Load (+ compile) shader files
	loadVertexShader(vsFilename);
	loadPixelShader(psFilename);

	// Setup the description of the dynamic matrix constant buffer that is in the vertex shader.
	matrixBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	matrixBufferDesc.ByteWidth = sizeof(MatrixBufferType);
	matrixBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	matrixBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	matrixBufferDesc.MiscFlags = 0;
	matrixBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&matrixBufferDesc, NULL, &matrixBuffer);

	// Create a texture sampler state description.
	samplerDesc.Filter = D3D11_FILTER_ANISOTROPIC;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
	renderer->CreateSamplerState(&samplerDesc, &sampleState);


	// Note that ByteWidth always needs to be a multiple of 16 if using D3D11_BIND_CONSTANT_BUFFER or CreateBuffer will fail.

	D3D11_BUFFER_DESC colourBufferDesc;
	colourBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	colourBufferDesc.ByteWidth = sizeof(ColourBufferType);
	colourBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	colourBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	colourBufferDesc.MiscFlags = 0;
	colourBufferDesc.StructureByteStride = 0;
	renderer->CreateBuffer(&colourBufferDesc, NULL, &colourBuffer);

}

void LineShader::SetShaderParameters(
	ID3D11DeviceContext* deviceContext,
	const XMMATRIX &worldMatrix,
	const XMMATRIX &viewMatrix,
	const XMMATRIX &projectionMatrix,
	ID3D11ShaderResourceView* texture
)
{
	HRESULT result;
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	MatrixBufferType* dataPtr;

	XMMATRIX tworld, tview, tproj;


	// Transpose the matrices to prepare them for the shader.
	tworld = XMMatrixTranspose(worldMatrix);
	tview = XMMatrixTranspose(viewMatrix);
	tproj = XMMatrixTranspose(projectionMatrix);
	result = deviceContext->Map(matrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	dataPtr = (MatrixBufferType*)mappedResource.pData;
	dataPtr->world = tworld;// worldMatrix;
	dataPtr->view = tview;
	dataPtr->projection = tproj;
	deviceContext->Unmap(matrixBuffer, 0);
	deviceContext->VSSetConstantBuffers(0, 1, &matrixBuffer);

	// Set shader texture resource in the pixel shader.
	deviceContext->PSSetShaderResources(0, 1, &texture);
	deviceContext->PSSetSamplers(0, 1, &sampleState);
}

void LineShader::SetColour(
	ID3D11DeviceContext* deviceContext,
	XMFLOAT4 colour
)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;

	ColourBufferType* colourPtr;
	deviceContext->Map(colourBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	colourPtr = (ColourBufferType*)mappedResource.pData;

	colourPtr->colour = colour;

	deviceContext->Unmap(colourBuffer, 0);
	deviceContext->PSSetConstantBuffers(0, 1, &colourBuffer);
}