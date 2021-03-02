#include "LineRenderer.h"

#include "MyClamp.h"

LineRenderer::LineRenderer(D3D* renderer, HWND hwnd)
	:
	shader(NULL),
	mesh(NULL)
{
	if (shader)
	{
		delete shader;
	}
	shader = new LineShader(renderer->getDevice(), hwnd);

	if (mesh)
	{
		delete mesh;
	}
	mesh = new LineMesh(renderer->getDevice(), renderer->getDeviceContext());
}

LineRenderer::~LineRenderer()
{
	if (shader)
	{
		delete shader;
	}

	if (mesh)
	{
		delete mesh;
	}
}

void LineRenderer::InitParameters(
	const XMFLOAT4& _lineColour
)
{
	lineColour = _lineColour;
}

void LineRenderer::Build(std::vector<Segment*>* segments)
{
	if (mesh)
	{
		mesh->Clear();
		for (Segment* seg : *segments)
		{
			mesh->AddLine(
				XMFLOAT3(
					seg->GetStartPoint().x,
					seg->GetStartPoint().y,
					seg->GetStartPoint().z
				),
				XMFLOAT3(
					seg->GetEndPoint().x,
					seg->GetEndPoint().y,
					seg->GetEndPoint().z
				),
				0
			);
		}
	}
}

void LineRenderer::SetShaderParams(
	const XMMATRIX& _worldMatrix,
	const XMMATRIX& _viewMatrix,
	const XMMATRIX& _projectionMatrix
)
{
	worldMatrix      = _worldMatrix;
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
}

void LineRenderer::RenderLines(D3D* renderer)
{
	if (shader && mesh)
	{
		int linesToRender = int(mesh->GetLineCount());

		for (int i = 0; i < linesToRender; i++)
		{
			shader->setShaderParameters(
				renderer->getDeviceContext(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				NULL,
				lineColour
			);

			mesh->sendData(renderer->getDeviceContext(), i);
			shader->render(renderer->getDeviceContext(), mesh->getIndexCount());
		}
	}
}