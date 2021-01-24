#include "CylinderMesh.h"

#include "MyMath.h"

CylinderMesh::CylinderMesh(ID3D11Device* device, ID3D11DeviceContext* deviceContext, float _bottomRadius, float _topRadius, float _resolution)
{
	bottomRadius = _bottomRadius;
	topRadius    = _topRadius;
	resolution   = _resolution;

	initBuffers(device);
}


CylinderMesh::~CylinderMesh()
{
	BaseMesh::~BaseMesh();
}

void CylinderMesh::InitBuffers(ID3D11Device* device) {

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
	float r = bottomRadius;
	float h = 0;

	//delta values:
	float deltaR = (bottomRadius - topRadius) / resolution;
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
		r = bottomRadius - (vertical * deltaR);

		for (int horizontal = 0; horizontal < resolution; horizontal++) {
			//starting a new "quad" (2 triangles):

			theta = horizontal * deltaTheta;
			uCoord = horizontal * deltaUV;

			//TOP LEFT:
			x = (r - deltaR) * cosf(theta);
			y = h + deltaHeight;
			z = (r - deltaR) * sinf(theta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord, (vCoord - deltaUV));
			//TODO CORRECT NORMALS FACTORING IN DELTA RADIUS!!
			vertices[v].normal = XMFLOAT3(x / r, 0.f, z / r);

			indices[i] = i;
			v++;
			i++;

			//BOTTOM LEFT:
			x = r * cosf(theta);
			y = h;
			z = r * sinf(theta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord, vCoord);
			//TODO CORRECT NORMALS FACTORING IN DELTA RADIUS!!
			vertices[v].normal = XMFLOAT3(x / r, 0.f, z / r);

			indices[i] = i;
			v++;
			i++;

			//TOP RIGHT:
			x = (r - deltaR) * cosf(theta + deltaTheta);
			y = h + deltaHeight;
			z = (r - deltaR) * sinf(theta + deltaTheta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord + deltaUV, vCoord - deltaUV);
			//TODO CORRECT NORMALS FACTORING IN DELTA RADIUS!!
			vertices[v].normal = XMFLOAT3(x / r, 0.f, z / r);

			indices[i] = i;
			v++;
			i++;


			//BOTTOM LEFT AGAIN:
			x = r * cosf(theta);
			y = h;
			z = r * sinf(theta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord, vCoord);
			//TODO CORRECT NORMALS FACTORING IN DELTA RADIUS!!
			vertices[v].normal = XMFLOAT3(x / r, 0.f, z / r);

			indices[i] = i;
			v++;
			i++;

			//BOTTOM RIGHT:
			x = r * cosf(theta + deltaTheta);
			y = h;
			z = r * sinf(theta + deltaTheta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord + deltaUV, vCoord);
			//TODO CORRECT NORMALS FACTORING IN DELTA RADIUS!!
			vertices[v].normal = XMFLOAT3(x / r, 0.f, z / r);

			indices[i] = i;
			v++;
			i++;

			//TOP RIGHT AGAN:
			x = (r - deltaR) * cosf(theta + deltaTheta);
			y = h + deltaHeight;
			z = (r - deltaR) * sinf(theta + deltaTheta);

			vertices[v].position = XMFLOAT3(x, y, z);
			vertices[v].texture = XMFLOAT2(uCoord + deltaUV, vCoord - deltaUV);
			//TODO CORRECT NORMALS FACTORING IN DELTA RADIUS!!
			vertices[v].normal = XMFLOAT3(x / r, 0.f, z / r);

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