#include "LineRenderer.h"

#include "Utils/MyVectorUtil.h"
#include "Maths/MyClamp.h"

////
// PUBLIC:
////

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

void LineRenderer::Build(std::vector<AnimSegment*>* animSegs)
{
	DeleteAllVectorData(&lines);

	for (AnimSegment* animSeg : *animSegs)
	{
		lines.push_back(new Line(animSeg));
	}
	
	if (mesh)
	{
		mesh->SetLines(&lines);
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

void LineRenderer::RenderLines(D3D* renderer, LightningRenderModes renderMode)
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

			mesh->sendData(renderMode, renderer->getDeviceContext(), i);
			shader->render(renderer->getDeviceContext(), mesh->getIndexCount());
		}
	}
}