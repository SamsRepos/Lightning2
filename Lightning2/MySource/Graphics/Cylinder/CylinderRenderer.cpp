#include "CylinderRenderer.h"

#include "Utils/MyVectorUtil.h"
#include "Maths/MyMath.h"
#include "Utils/DxColourLerp.h"
#include "Maths/MyClamp.h"

////
// PUBLIC:
////

CylinderRenderer::CylinderRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight)
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
	mainShader    = new CylinderShader(renderer->getDevice(), hwnd);

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

	energyTexture = new RenderTexture(
		renderer->getDevice(),
		screenWidth,
		screenHeight,
		SCREEN_NEAR,
		SCREEN_DEPTH
	);

	//

	fullScreenMesh = new OrthoMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		screenWidth,
		screenHeight
	);
}

CylinderRenderer::~CylinderRenderer()
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

	if (energyTexture)
	{
		delete energyTexture;
		energyTexture = NULL;
	}

	if (fullScreenMesh)
	{
		delete fullScreenMesh;
		fullScreenMesh = NULL;
	}

	ClearCylinders();
}

void CylinderRenderer::SetColours(
	const XMFLOAT4& _backgroundColour,
	const XMFLOAT4& _blurColour,
	const XMFLOAT4& _cylinderColour
)
{
	backgroundColour = _backgroundColour;
	blurColour       = _blurColour;
	cylinderColour   = _cylinderColour;
	
}

void CylinderRenderer::SetBlurParams(
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

void CylinderRenderer::Build(std::vector<AnimSegment*>* animSegs)
{
	ClearCylinders();
	
	for (AnimSegment* animSeg : *animSegs)
	{
		CapsuleObject* newCylinder = new CapsuleObject(*baseCapsule);
		newCylinder->Init(animSeg);

		float maxEnergy = MaxEnergy(animSegs);
		float maxEnergyLogE = log(maxEnergy);
		float maxEnergyLog10 = log10(maxEnergy);

		newCylinder->SetBrightness(
			MyClamp(
				//(log10(animSeg->GetEnergy()) / maxEnergyLog10),
				//(log(animSeg->GetEnergy()) / maxEnergyLogE),
				(animSeg->GetEnergy() / maxEnergy),
				0.f,
				1.f
			)
		);

		capsuleObjects.push_back(newCylinder);
	}	
}

void CylinderRenderer::UpdateFromAnimation()
{
	for (CapsuleObject* cyl : capsuleObjects)
	{
		cyl->UpdateFromAnimation();
	}
}

void CylinderRenderer::SetShaderParams(const XMMATRIX& _viewMatrix,	const XMMATRIX& _projectionMatrix)
{
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
}

void CylinderRenderer::RenderBlur(D3D* renderer, Camera* camera, LightningRenderModes renderMode)
{
	XMMATRIX worldMatrix     = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix(); // Default camera position for orthographic rendering
	XMMATRIX orthoMatrix     = renderer->getOrthoMatrix();   // ortho matrix for 2D rendering
	
	//0. Energy pass:
	energyTexture->setRenderTarget(renderer->getDeviceContext());
	energyTexture->clearRenderTarget(
		renderer->getDeviceContext(),
		0.f, 0.f, 0.f, 1.f
	);

	for (CapsuleObject* c : capsuleObjects)
	{
		if (
			(renderMode == LightningRenderModes::ANIMATED && c->IsVisible()) ||
			renderMode == LightningRenderModes::STATIC
			)
		{
			// Colour is brightness, IE relative energy (0 <= e <= 1)
			XMFLOAT4 colour = XMFLOAT4(
				c->GetBrightness(),
				c->GetBrightness(),
				c->GetBrightness(),
				1.f
			);

			c->GetMesh()->sendData(renderer->getDeviceContext());
			mainShader->setShaderParameters(renderer->getDeviceContext(), c->GetTransform(), viewMatrix, projectionMatrix, c->GetTexture(), colour);
			mainShader->render(renderer->getDeviceContext(), c->GetMesh()->getIndexCount());
		}
	}



	//1. Render cylinders to the blur texture:
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
		if (
			(renderMode == LightningRenderModes::ANIMATED && c->IsVisible()) ||
			renderMode == LightningRenderModes::STATIC
			)
		{
			XMFLOAT4 colour = blurColour; /* DxColourLerp(
				backgroundColour,
				blurColour,
				c->GetBrightness()
			);*/

			c->GetMesh()->sendData(renderer->getDeviceContext());
			mainShader->setShaderParameters(renderer->getDeviceContext(), c->GetTransform(), viewMatrix, projectionMatrix, c->GetTexture(), colour);
			mainShader->render(renderer->getDeviceContext(), c->GetMesh()->getIndexCount());
		}
	}

	//2. Render blur texture for blur pass:
	blurRenderTexture2->setRenderTarget(renderer->getDeviceContext());
	blurRenderTexture2->clearRenderTarget(
		renderer->getDeviceContext(),
		backgroundColour.x,
		backgroundColour.y,
		backgroundColour.z,
		backgroundColour.w
	);

	renderer->setZBuffer(false);

	blurShader->setScreenSize(renderer->getDeviceContext(), XMINT2(screenWidth, screenHeight));
	fullScreenMesh->sendData(renderer->getDeviceContext());
	blurShader->setShaderParameters(
		renderer->getDeviceContext(),
		worldMatrix,
		orthoViewMatrix,
		orthoMatrix,
		blurRenderTexture1->getShaderResourceView()
	);

	blurShader->updateBlurParameters(
		renderer->getDeviceContext(),
		energyTexture,
		blurDirections,
		blurQuality,
		blurSize,
		blurFinalAdjustment
	);

	blurShader->render(
		renderer->getDeviceContext(),
		fullScreenMesh->getIndexCount()
	);

	//3. Render blur texture:
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

void CylinderRenderer::RenderCylinders(D3D* renderer, LightningRenderModes renderMode)
{
	for (CapsuleObject* c : capsuleObjects)
	{
		if (
			(renderMode == LightningRenderModes::ANIMATED && c->IsVisible()) ||
			renderMode==LightningRenderModes::STATIC
		)
		{
			XMFLOAT4 colour = cylinderColour;
			
			//DxColourLerp(
			//	backgroundColour,
			//	cylinderColour,
			//	c->GetBrightness()
			//);

			c->GetMesh()->sendData(renderer->getDeviceContext());
			mainShader->setShaderParameters(renderer->getDeviceContext(), c->GetTransform(), viewMatrix, projectionMatrix, c->GetTexture(), colour);
			mainShader->render(renderer->getDeviceContext(), c->GetMesh()->getIndexCount());

			c->GetDomeMesh()->sendData(renderer->getDeviceContext());
			mainShader->setShaderParameters(
				renderer->getDeviceContext(),
				c->GetDomeTransform1(),
				viewMatrix,
				projectionMatrix,
				c->GetTexture(),
				colour
			);
			mainShader->render(renderer->getDeviceContext(), c->GetDomeMesh()->getIndexCount());

			c->GetDomeMesh()->sendData(renderer->getDeviceContext());
			mainShader->setShaderParameters(
				renderer->getDeviceContext(),
				c->GetDomeTransform2(),
				viewMatrix,
				projectionMatrix,
				c->GetTexture(),
				colour
			);
			mainShader->render(renderer->getDeviceContext(), c->GetDomeMesh()->getIndexCount());

		}
	}
}

void CylinderRenderer::ClearCylinders()
{
	DeleteAllVectorData(&capsuleObjects);
}

////
// PRIVATE:
////

float CylinderRenderer::MaxEnergy(std::vector<AnimSegment*>* animSegs)
{
	float maxEnergy = 0.f;
	for (AnimSegment* animSeg : *animSegs)
	{
		if (animSeg->GetEnergy() > maxEnergy)
		{
			maxEnergy = animSeg->GetEnergy();
		}
	}
	return maxEnergy;
}