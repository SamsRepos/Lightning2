#include "CapsuleRenderer.h"

#include "Utils/MyVectorUtil.h"
#include "Maths/MyMath.h"
#include "Utils/DxColourLerp.h"
#include "Maths/MyClamp.h"

////
// PUBLIC:
////

CapsuleRenderer::CapsuleRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight)
	:
	cylinderMesh(NULL),
	domeMesh(NULL),
	baseCapsule(NULL)
{
	cylinderMesh = new CylinderMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		.5f, //diameter of 1, which matches default height
		10 // resolution
	);

	domeMesh = new DomeMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		.5f,
		10
	);

	//Scene objects:
	baseCapsule = new CapsuleObject(cylinderMesh, domeMesh, NULL, renderer->getWorldMatrix());

	//Shaders:
	blurShader    = new BlurShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	mainShader    = new CapsuleShader(renderer->getDevice(), hwnd);

	screenWidth  = _screenWidth;
	screenHeight = _screenHeight;

	//Render textures:
	blurRenderTexture1 = new RenderTexture(
		renderer->getDevice(),
		screenWidth,
		screenHeight,
		SCREEN_NEAR,
		SCREEN_DEPTH
	);
	blurRenderTexture2 = new RenderTexture(
		renderer->getDevice(),
		screenWidth,
		screenHeight,
		SCREEN_NEAR,
		SCREEN_DEPTH
	);

	fullScreenMesh = new OrthoMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		screenWidth,
		screenHeight
	);
}

CapsuleRenderer::~CapsuleRenderer()
{
	if (cylinderMesh)
	{
		delete cylinderMesh;
		cylinderMesh = NULL;
	}

	if (domeMesh)
	{
		delete domeMesh;
		domeMesh = NULL;
	}

	if (baseCapsule)
	{
		delete baseCapsule;
		baseCapsule = NULL;
	}

	if (blurShader)
	{
		delete blurShader;
		blurShader = NULL;
	}

	if (textureShader)
	{
		delete textureShader;
		textureShader = NULL;
	}

	if (mainShader)
	{
		delete mainShader;
		mainShader = NULL;
	}

	if (blurRenderTexture1)
	{
		delete blurRenderTexture1;
		blurRenderTexture1 = NULL;
	}

	if (blurRenderTexture2)
	{
		delete blurRenderTexture2;
		blurRenderTexture2 = NULL;
	}
	
	if (fullScreenMesh)
	{
		delete fullScreenMesh;
		fullScreenMesh = NULL;
	}

	ClearCapsules();
}

void CapsuleRenderer::SetColours(
	const XMFLOAT4& _backgroundColour,
	const XMFLOAT4& _blurColour,
	const XMFLOAT4& _cylinderColour
)
{
	backgroundColour = _backgroundColour;
	blurColour       = _blurColour;
	cylinderColour   = _cylinderColour;
	
}

void CapsuleRenderer::SetBlurParams(
	float _blurDirections,
	float _blurQuality,
	float _blurSize,
	float _blurFinalAdjustment
)
{
	blurDirections      = _blurDirections;
	blurQuality         = _blurQuality;
	blurSize            = _blurSize;
	blurFinalAdjustment = _blurFinalAdjustment;
}

void CapsuleRenderer::Build(std::vector<AnimSegment*>* animSegs)
{
	ClearCapsules();
	
	for (AnimSegment* animSeg : *animSegs)
	{
		CapsuleObject* newCapsule = new CapsuleObject(*baseCapsule);
		newCapsule->Init(animSeg);

		capsuleObjects.push_back(newCapsule);
	}	
}

void CapsuleRenderer::InitStatic()
{
	for (CapsuleObject* c : capsuleObjects)
	{
		c->InitStatic();
	}
}

void CapsuleRenderer::UpdateFromAnimation()
{
	for (CapsuleObject* c : capsuleObjects)
	{
		c->UpdateFromAnimation();
	}
}

void CapsuleRenderer::SetShaderParams(const XMMATRIX& _viewMatrix,	const XMMATRIX& _projectionMatrix)
{
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
}

void CapsuleRenderer::RenderBlur(D3D* renderer, Camera* camera, LightningRenderModes renderMode, bool energyForBlur)
{
	XMMATRIX worldMatrix     = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix(); // Default camera position for orthographic rendering
	XMMATRIX orthoMatrix     = renderer->getOrthoMatrix();   // ortho matrix for 2D rendering
	
	//1. Initial pass (capsules -> texture):
	blurRenderTexture1->setRenderTarget(renderer->getDeviceContext());
	blurRenderTexture1->clearRenderTarget(
		renderer->getDeviceContext(),
		backgroundColour.x,
		backgroundColour.y,
		backgroundColour.z,
		backgroundColour.w
	);
		
	for (CapsuleObject* c : capsuleObjects)
	{
		if (ShouldBeRendered(renderMode, c))
		{
			XMFLOAT4 colour;

			if (energyForBlur)
			{
				colour = DxColourLerp(
					backgroundColour,
					blurColour,
					c->GetBrightness()
				);
			}
			else
			{
				colour = blurColour;
			}

			mainShader->SetColour(renderer->getDeviceContext(), colour);
						
			c->Render(
				mainShader,
				renderer,
				viewMatrix,
				projectionMatrix,
				colour
			);
		}
	}

	//2. Blur pass (texture -> texture):
	blurRenderTexture2->setRenderTarget(renderer->getDeviceContext());
	blurRenderTexture2->clearRenderTarget(
		renderer->getDeviceContext(),
		backgroundColour.x,
		backgroundColour.y,
		backgroundColour.z,
		backgroundColour.w
	);

	renderer->setZBuffer(false);

	blurShader->SetScreenSize(renderer->getDeviceContext(), XMINT2(screenWidth, screenHeight));
	fullScreenMesh->sendData(renderer->getDeviceContext());
	blurShader->SetShaderParameters(
		renderer->getDeviceContext(),
		worldMatrix,
		orthoViewMatrix,
		orthoMatrix,
		blurRenderTexture1->getShaderResourceView()
	);

	blurShader->SetBlurParameters(
		renderer->getDeviceContext(),
		blurDirections,
		blurQuality,
		blurSize,
		blurFinalAdjustment
	);

	blurShader->render(
		renderer->getDeviceContext(),
		fullScreenMesh->getIndexCount()
	);

	//3. Final pass (texture -> backbuffer):
	renderer->setBackBufferRenderTarget();

	fullScreenMesh->sendData(renderer->getDeviceContext());
	textureShader->setShaderParameters(
		renderer->getDeviceContext(),
		worldMatrix,
		orthoViewMatrix,
		orthoMatrix,
		blurRenderTexture2->getShaderResourceView()
	);
	textureShader->render(renderer->getDeviceContext(), fullScreenMesh->getIndexCount());

	renderer->setZBuffer(true);
}

void CapsuleRenderer::RenderCapsules(D3D* renderer, LightningRenderModes renderMode, bool energyForBrightness)
{
	for (CapsuleObject* c : capsuleObjects)
	{
		if (ShouldBeRendered(renderMode, c))
		{
			XMFLOAT4 colour;

			if (energyForBrightness)
			{
				colour = DxColourLerp(
					backgroundColour,
					cylinderColour,
					c->GetBrightness()
				);
			}
			else
			{
				colour = cylinderColour;
			}

			mainShader->SetColour(renderer->getDeviceContext(), colour);
			c->Render(
				mainShader,
				renderer,
				viewMatrix,
				projectionMatrix,
				colour
			);

		}
	}
}

void CapsuleRenderer::ClearCapsules()
{
	DeleteAllVectorData(&capsuleObjects);
}