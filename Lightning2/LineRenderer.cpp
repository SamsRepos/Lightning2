#include "LineRenderer.h"

#include "MyClamp.h"

LineRenderer::LineRenderer()
	:
	shader(NULL),
	mesh(NULL)
{
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

void LineRenderer::Init(D3D* renderer, HWND hwnd)
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
	const XMMATRIX& _projectionMatrix,
	Light* _light,
	const XMFLOAT4& _colour
)
{
	worldMatrix      = _worldMatrix;
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
	light            = _light;
	colour           = _colour;
}

void LineRenderer::Render(D3D* renderer)
{
	if (shader && mesh && light)
	{
		//linesToRender = MyClamp(linesToRender, 0, int(mesh->GetLineCount()));
		linesToRender = int(mesh->GetLineCount());

		for (int i = 0; i < linesToRender; i++)
		{
			shader->setShaderParameters(
				renderer->getDeviceContext(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				NULL,
				light,
				colour
			);

			mesh->sendData(renderer->getDeviceContext(), i);
			shader->render(renderer->getDeviceContext(), mesh->getIndexCount());
		}
	}
}