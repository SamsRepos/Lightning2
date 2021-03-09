#include "LightningRenderer.h"

#include "Utils/MyVectorUtil.h"

////
// PUBLIC:
////

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
	cylRenderer  = new CylinderRenderer(renderer, hwnd, screenWidth, screenHeight);

	blurRenderingActive = true;
	lineRenderingActive = true;
	cylinderRenderingActive = true;
}

LightningRenderer::~LightningRenderer()
{
	Clear();

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
	Clear();

	Segment* rootSeg = segs->front();

	CreateAnimSegmentsRecurs(rootSeg, NULL);

	/*float maxEnergy = 0.f;
	for (Segment* seg : *segs)
	{
		if (seg->GetEnergy() > maxEnergy);
		maxEnergy = seg->GetEnergy();
	}*/

	lineRenderer->Build(&animSegments);
	cylRenderer->Build(&animSegments);
}

void LightningRenderer::InitAnimation()
{
	animatingNow = true;

	for (AnimSegment* animSeg : animSegments)
	{
		animSeg->InitAnimation();
	}

	AnimSegment* rootAnimSeg = animSegments.front();
	rootAnimSeg->SetVisible(true);
}

// Returns true if still animating
bool LightningRenderer::UpdateAnimation(float dt)
{
	if (animSegments.size() > 0 && animatingNow)
	{
		AnimSegment* rootAnimSeg = animSegments.front();
		animatingNow = rootAnimSeg->UpdateAnimationRecurs(dt * animationSpeed);		
	}
	return animatingNow;
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
		if (renderMode == LightningRenderModes::ANIMATED)
		{
			cylRenderer->UpdateFromAnimation();
		}
		cylRenderer->SetShaderParams(viewMatrix, projMatrix);
	}

	if (blurRenderingActive)
	{
		cylRenderer->RenderBlur(renderer, camera, renderMode);
	}

	if (lineRenderingActive)
	{
		lineRenderer->SetShaderParams(worldMatrix, viewMatrix, projMatrix);
		lineRenderer->RenderLines(renderer, renderMode);
	}

	if (cylinderRenderingActive)
	{
		cylRenderer->RenderCylinders(renderer, renderMode);
	}
}

void LightningRenderer::Clear()
{
	DeleteAllVectorData(&animSegments);

	lineRenderer->ClearLines();
	cylRenderer->ClearCylinders();
}

////
// PRIVATE:
////

void LightningRenderer::CreateAnimSegmentsRecurs(Segment* seg, AnimSegment* parent)
{
	AnimSegment* newAnimSeg = new AnimSegment(seg);

	if (parent)
	{
		parent->AddChild(newAnimSeg);
		newAnimSeg->SetParent(parent);
	}

	animSegments.push_back(newAnimSeg);

	for (Segment* s : *(seg->GetChildren()))
	{
		CreateAnimSegmentsRecurs(s, newAnimSeg);
	}
}