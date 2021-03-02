#include "CylinderMesh.h"

#include "MyMath.h"

CylinderMesh::CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float _radius, float _resolution)
	:
	radius(_radius),
	resolution(_resolution)
{
	initBuffers(device);
}


CylinderMesh::~CylinderMesh()
{
	BaseMesh::~BaseMesh();
}

void CylinderMesh::initBuffers(ID3D11Device* device) {

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

	//variables to build the cylinder, starting from the bottom:
	//starting positions:
	float x, y, z;
	float theta = 0.f;
	float h = 0;

	//delta values:
	float deltaHeight = 1.f / resolution;
	float deltaTheta = (2 * PI) / resolution;

	//UVs:
	float uCoord, vCoord;
	uCoord = 0.f;
	vCoord = 1.f;
	float deltaUV = 1.f / resolution;

	for (int vertical = 0; vertical < resolution; vertical++) {
		//starting a new stack:
		h = deltaHeight * vertical;
		vCoord = 1.f - (vertical * deltaUV);

		for (int horizontal = 0; horizontal < resolution; horizontal++) {
			//starting a new "quad" (2 triangles):

			theta = horizontal * deltaTheta;
			uCoord = horizontal * deltaUV;

			//TRIANGLE 1:
			//TOP LEFT:
			x = radius * cosf(theta);
			y = h + deltaHeight;
			z = radius * sinf(theta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture  = XMFLOAT2(uCoord, (vCoord - deltaUV));			
			vertices[v].normal   = XMFLOAT3(x / radius, 0.f, z / radius);

			indices[i] = i;
			v++;
			i++;

			//BOTTOM LEFT:
			x = radius * cosf(theta);
			y = h;
			z = radius * sinf(theta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture  = XMFLOAT2(uCoord, vCoord);
			vertices[v].normal   = XMFLOAT3(x / radius, 0.f, z / radius);

			indices[i] = i;
			v++;
			i++;

			//TOP RIGHT:
			x = radius * cosf(theta + deltaTheta);
			y = h + deltaHeight;
			z = radius * sinf(theta + deltaTheta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture  = XMFLOAT2(uCoord + deltaUV, vCoord - deltaUV);
			vertices[v].normal   = XMFLOAT3(x / radius, 0.f, z / radius);

			indices[i] = i;
			v++;
			i++;
			
			//TRIANGLE 2:
			//BOTTOM LEFT AGAIN:
			x = radius * cosf(theta);
			y = h;
			z = radius * sinf(theta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord, vCoord);
			vertices[v].normal = XMFLOAT3(x / radius, 0.f, z / radius);

			indices[i] = i;
			v++;
			i++;

			//BOTTOM RIGHT:
			x = radius * cosf(theta + deltaTheta);
			y = h;
			z = radius * sinf(theta + deltaTheta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord + deltaUV, vCoord);
			vertices[v].normal = XMFLOAT3(x / radius, 0.f, z / radius);

			indices[i] = i;
			v++;
			i++;

			//TOP RIGHT AGAN:
			x = radius * cosf(theta + deltaTheta);
			y = h + deltaHeight;
			z = radius * sinf(theta + deltaTheta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord + deltaUV, vCoord - deltaUV);
			vertices[v].normal = XMFLOAT3(x / radius, 0.f, z / radius);

			indices[i] = i;
			v++;
			i++;
		}

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