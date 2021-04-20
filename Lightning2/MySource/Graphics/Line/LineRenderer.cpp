#include "LineRenderer.h"

#include "Utils/MyVectorUtil.h"
#include "Utils/DxColourLerp.h"
#include "Maths/MyClamp.h"
#include "Utils/EnergyToBrightness.h"

////
// PUBLIC:
////

LineRenderer::LineRenderer(D3D* renderer, HWND hwnd)
	:
	shader(NULL),
	renderer(renderer)
{
	if (shader)
	{
		delete shader;
	}
	shader = new LineShader(renderer->getDevice(), hwnd);		
}

LineRenderer::~LineRenderer()
{
	if (shader)
	{
		delete shader;
	}

	ClearLines();
}
void LineRenderer::SetColours(
	const XMFLOAT4& _backgroundColour,
	const XMFLOAT4& _lineColour
) {
	backgroundColour = _backgroundColour;
	lineColour = _lineColour;
};

void LineRenderer::Build(std::vector<AnimSegment*>* animSegs, EnergyScales energyScale)
{
	ClearLines();

	for (AnimSegment* animSeg : *animSegs)
	{
		LineMesh* newLine = new LineMesh(renderer->getDevice(), renderer->getDeviceContext(), animSeg);

		float maxEnergy = MaxEnergy(animSegs);
		float capsuleBrightness = EnergyToBrightness(animSeg->GetEnergy(), maxEnergy, energyScale);
		newLine->SetBrightness(capsuleBrightness);

		lines.push_back(newLine);
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

void LineRenderer::RenderLines(D3D* renderer, LightningRenderModes renderMode, bool energyForBrightness)
{
	if (shader)
	{
		for (LineMesh* line : lines)
		{
			shader->SetShaderParameters(
				renderer->getDeviceContext(),
				worldMatrix,
				viewMatrix,
				projectionMatrix,
				NULL
			);
			if (energyForBrightness)
			{
				shader->SetColour(
					renderer->getDeviceContext(),
					DxColourLerp(
						backgroundColour,
						lineColour,
						line->GetBrightness()
					)
				);
			}
			else
			{
				shader->SetColour(
					renderer->getDeviceContext(),
					lineColour
				);
			}

			line->sendData(renderMode, renderer->getDeviceContext(), 0);
			shader->render(renderer->getDeviceContext(), line->getIndexCount());
		}
	}
}

void LineRenderer::ClearLines()
{
	DeleteAllVectorData(&lines);
}