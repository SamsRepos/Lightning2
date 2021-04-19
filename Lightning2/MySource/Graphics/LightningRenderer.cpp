#include "LightningRenderer.h"

#include "DefaultParameters.h"
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
	capRenderer(NULL)
{
	lineRenderer = new LineRenderer(renderer, hwnd);
	capRenderer  = new CapsuleRenderer(renderer, hwnd, screenWidth, screenHeight);

	blurRenderingActive = true;
	lineRenderingActive = true;
	capsuleRenderingActive = true;
}

LightningRenderer::~LightningRenderer()
{
	Clear();

	if (lineRenderer)
	{
		delete lineRenderer;
		lineRenderer = NULL;
	}
	if (capRenderer)
	{
		delete capRenderer;
		capRenderer = NULL;
	}
}

void LightningRenderer::SetRenderMode(LightningRenderModes mode)
{
	renderMode = mode; 
	if (renderMode == LightningRenderModes::STATIC)
	{
		capRenderer->InitStatic();
	}
};

void LightningRenderer::SetColours(
	const XMFLOAT4& backgroundColour,
	const XMFLOAT4& blurColour,
	const XMFLOAT4& lineColour,
	const XMFLOAT4& cylinderColour
)
{
	lineRenderer->SetColour(lineColour);

	capRenderer->SetColours(
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
	capRenderer->SetBlurParams(
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
	recursCapHit = false;

	Clear();

	Segment* rootSeg = segs->front();

	CreateAnimSegmentsRecurs(rootSeg, NULL, 0);

	/*float maxEnergy = 0.f;
	for (Segment* seg : *segs)
	{
		if (seg->GetEnergy() > maxEnergy);
		maxEnergy = seg->GetEnergy();
	}*/

	lineRenderer->Build(&animSegments);
	capRenderer->Build(&animSegments);
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
	if (blurRenderingActive || capsuleRenderingActive)
	{
		if (renderMode == LightningRenderModes::ANIMATED)
		{
			capRenderer->UpdateFromAnimation();
		}
		capRenderer->SetShaderParams(viewMatrix, projMatrix);
	}

	if (blurRenderingActive)
	{
		capRenderer->RenderBlur(renderer, camera, renderMode);
	}

	if (lineRenderingActive)
	{
		lineRenderer->SetShaderParams(worldMatrix, viewMatrix, projMatrix);
		lineRenderer->RenderLines(renderer, renderMode);
	}

	if (capsuleRenderingActive)
	{
		capRenderer->RenderCapsules(renderer, renderMode);
	}
}

void LightningRenderer::Clear()
{
	DeleteAllVectorData(&animSegments);

	lineRenderer->ClearLines();
	capRenderer->ClearCapsules();
}

////
// PRIVATE:
////

void LightningRenderer::CreateAnimSegmentsRecurs(Segment* seg, AnimSegment* parent, size_t recursCount)
{
	AnimSegment* newAnimSeg = new AnimSegment(seg);

	if (parent)
	{
		parent->AddChild(newAnimSeg);
		newAnimSeg->SetParent(parent);
	}

	animSegments.push_back(newAnimSeg);

	if (recursCount < RECURSIVE_CAP)
	{
		for (Segment* s : *(seg->GetChildren()))
		{
			CreateAnimSegmentsRecurs(s, newAnimSeg, recursCount+1);
		}
	}
	else
	{
		recursCapHit = true;
	}
}