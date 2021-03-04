#include "LightningRenderer.h"

LightningRenderer::LightningRenderer(
	D3D* renderer,
	HWND hwnd,
	int screenWidth,
	int screenHeight
)
	:
	renderer(renderer),
	lineRenderer(NULL),
	cylRenderer(NULL)
{
	lineRenderer = new LineRenderer(renderer, hwnd);
	cylRenderer = new CylinderRenderer(renderer, hwnd, screenWidth, screenHeight);

	blurRenderingActive = true;
	lineRenderingActive = true;
	cylinderRenderingActive = true;
}

LightningRenderer::~LightningRenderer()
{
	if (lineRenderer)
	{
		delete lineRenderer;
		lineRenderer = NULL;
	}
	if (cylRenderer)
	{
		delete cylRenderer;
		cylRenderer = NULL;
	}
}

void LightningRenderer::SetColours(
	const XMFLOAT4& backgroundColour,
	const XMFLOAT4& blurColour,
	const XMFLOAT4& lineColour,
	const XMFLOAT4& cylinderColour
)
{
	lineRenderer->SetColour(lineColour);

	cylRenderer->SetColours(
		backgroundColour,
		blurColour,
		cylinderColour
	);
}

void LightningRenderer::SetBlurParams(
	float blurDirections,
	float blurQuality,
	float blurSize,
	float blurAdjustment
)
{
	cylRenderer->SetBlurParams(
		blurDirections,
		blurQuality,
		blurSize,
		blurAdjustment
	);
}

void LightningRenderer::SetAnimationParams(
	float speed
)
{
	animationSpeed = speed;
}

void LightningRenderer::Build(std::vector<Segment*>* segs)
{
	float maxEnergy = 0.f;
	for (Segment* seg : *segs)
	{
		if (seg->GetEnergy() > maxEnergy);
		maxEnergy = seg->GetEnergy();
	}

	lineRenderer->Build(segs);
	cylRenderer->Build(segs, maxEnergy);
}

void LightningRenderer::InitAnimation()
{
	cylRenderer->InitAnimation();
}

void LightningRenderer::UpdateAnimation(float dt)
{
	cylRenderer->UpdateAnimation(dt * animationSpeed);
}

void LightningRenderer::Render(
	Camera* camera,
	const XMMATRIX& worldMatrix,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projMatrix
)
{
	if (blurRenderingActive || cylinderRenderingActive)
	{
		cylRenderer->SetShaderParams(viewMatrix, projMatrix);
	}

	if (blurRenderingActive)
	{
		cylRenderer->RenderBlur(renderer, camera);
	}

	if (lineRenderingActive)
	{
		lineRenderer->SetShaderParams(worldMatrix, viewMatrix, projMatrix);
		lineRenderer->RenderLines(renderer);
	}

	if (cylinderRenderingActive)
	{
		cylRenderer->RenderCylinders(renderer);
	}
}