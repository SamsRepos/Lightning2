#include "DomeMesh.h"

#include "Maths/MyMath.h"

DomeMesh::DomeMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float _radius, float _resolution)
	:
	radius(_radius),
	resolution(_resolution)
{
	initBuffers(device);
}


DomeMesh::~DomeMesh()
{
	BaseMesh::~BaseMesh();
}

void DomeMesh::initBuffers(ID3D11Device* device) {

	VertexType* vertices;
	unsigned long* indices;
	D3D11_BUFFER_DESC vertexBufferDesc, indexBufferDesc;
	D3D11_SUBRESOURCE_DATA vertexData, indexData;

	//resolution num stacks * resolution num 
	vertexCount = resolution * resolution * 6;
	indexCount = vertexCount;

	vertices = new VertexType[vertexCount];
	indices = new unsigned long[indexCount];

	int v = 0; //the vertex counter
	int i = 0; //the index counter

	//variables to build the dome:
	float x, y, z;
	float theta, phi;
	//intial values:
	x = y = z = 0.f;
	theta = phi = 0.f;

	//delta values:
	float deltaTheta = (TWO_PI) / resolution;
	float deltaPhi = (PI_OVER_TWO) / resolution;

	//UVs:
	float uCoord, vCoord;
	uCoord = vCoord = 0.f;
	float deltaUV = 1.f / resolution;

	// Longitude loop:
	for (float theta = 0.f; theta < TWO_PI; theta += deltaTheta)
	{
		// Latitude loop:
		for (float phi = 0.f; phi < PI_OVER_TWO; phi += deltaPhi)
		{
			float nextTheta = theta + deltaTheta;
			float nextPhi = phi + deltaPhi;

			float nextUCoord = uCoord + deltaUV;
			float nextVCoord = vCoord + deltaUV;

			//TRIANGLE 1:
			//TOP LEFT:
			x = radius * cosf(nextTheta) * sinf(phi);
			y = radius * cosf(phi);
			z = radius * sinf(nextTheta) * sinf(phi);

			vertices[v].position = XMFLOAT3(x, y, z);
			
			vertices[v].texture = XMFLOAT2(uCoord, nextVCoord);

			vertices[v].normal = XMFLOAT3(x / radius, y / radius, z / radius);

			indices[i] = i;
			i++;
			v++;

			//BOTTOM LEFT:
			x = radius * cosf(theta) * sinf(phi);
			y = radius * cosf(phi);
			z = radius * sinf(theta) * sinf(phi);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord, vCoord);
			vertices[v].normal = XMFLOAT3(x / radius, y / radius, z / radius);

			indices[i] = i;
			i++;
			v++;

			//TOP RIGHT:
			x = radius * cosf(nextTheta) * sinf(nextPhi);
			y = radius * cosf(nextPhi);
			z = radius * sinf(nextTheta) * sinf(nextPhi);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(nextUCoord, nextVCoord);
			vertices[v].normal = XMFLOAT3(x / radius, y / radius, z / radius);

			indices[i] = i;
			i++;
			v++;

			//TRIANGLE 2:
			//BOTTOM LEFT AGAIN:
			x = radius * cosf(theta) * sinf(phi);
			y = radius * cosf(phi);
			z = radius * sinf(theta) * sinf(phi);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord, vCoord);
			vertices[v].normal = XMFLOAT3(x / radius, y / radius, z / radius);

			indices[i] = i;
			i++;
			v++;

			//BOTTOM RIGHT:
			x = radius * cosf(theta) * sinf(nextPhi);
			y = radius * cosf(nextPhi);
			z = radius * sinf(theta) * sinf(nextPhi);

			vertices[v].position = XMFLOAT3(x, y, z);
			
			vertices[v].texture = XMFLOAT2(nextUCoord, vCoord);

			vertices[v].normal = XMFLOAT3(x / radius, y / radius, z / radius);

			indices[i] = i;
			i++;
			v++;

			//TOP RIGHT AGAIN:
			x = radius * cosf(nextTheta) * sinf(nextPhi);
			y = radius * cosf(nextPhi);
			z = radius * sinf(nextTheta) * sinf(nextPhi);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(nextUCoord, nextVCoord);
			vertices[v].normal = XMFLOAT3(x / radius, y / radius, z / radius);

			indices[i] = i;
			i++;
			v++;

			// end of latitude loop
			uCoord += deltaUV;
		}
		// end of longituude loop
		uCoord = 0.f;
		vCoord += deltaUV;
	}

	// Set up the description of the static vertex buffer.
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexType) * vertexCount;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;
	vertexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the vertex data.
	vertexData.pSysMem = vertices;
	vertexData.SysMemPitch = 0;
	vertexData.SysMemSlicePitch = 0;
	// Now create the vertex buffer.
	device->CreateBuffer(&vertexBufferDesc, &vertexData, &vertexBuffer);

	// Set up the description of the static index buffer.
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(unsigned long) * indexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;
	indexBufferDesc.StructureByteStride = 0;
	// Give the subresource structure a pointer to the index data.
	indexData.pSysMem = indices;
	indexData.SysMemPitch = 0;
	indexData.SysMemSlicePitch = 0;
	// Create the index buffer.
	device->CreateBuffer(&indexBufferDesc, &indexData, &indexBuffer);

	// Release the arrays now that the vertex and index buffers have been created and loaded.
	delete[] vertices;
	vertices = 0;

	delete[] indices;
	indices = 0;
}