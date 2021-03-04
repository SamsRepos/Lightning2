#include "CylinderRenderer.h"

#include "MyVectorUtil.h"
#include "MyMath.h"
#include "DxColourLerp.h"
#include "MyClamp.h"

CylinderRenderer::CylinderRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight)
	:
	cylinderMesh(NULL),
	baseCylinder(NULL)
{
	cylinderMesh = new CylinderMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		.5f, //diameter of 1, which matches default height
		5 // resolution
	);

	//Scene objects:
	baseCylinder = new CylinderObject(cylinderMesh, NULL, renderer->getWorldMatrix());

	//Shaders:
	blurShader    = new BlurShader(renderer->getDevice(), hwnd);
	textureShader = new TextureShader(renderer->getDevice(), hwnd);
	mainShader    = new CylinderShader(renderer->getDevice(), hwnd);

	screenWidth  = _screenWidth;
	screenHeight = _screenHeight;

	//Render texture:	
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

CylinderRenderer::~CylinderRenderer()
{
	cylinderObjects.clear();

	if (cylinderMesh)
	{
		delete cylinderMesh;
		cylinderMesh = NULL;
	}

	if (baseCylinder)
	{
		delete baseCylinder;
		baseCylinder = NULL;
	}

	DeleteAllVectorData(&cylinderObjects);
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

void CylinderRenderer::Build(std::vector<Segment*>* segments, float maxEnergy)
{
	DeleteAllVectorData(&cylinderObjects);

	Segment* rootSeg = segments->front();

	CreateCylinderRecurs(rootSeg, NULL);	
}

void CylinderRenderer::InitAnimation()
{
	animatingNow = true;

	for (CylinderObject* cyl : cylinderObjects)
	{
		cyl->InitAnimation();
	}

	CylinderObject* rootCyl = cylinderObjects.front();
	rootCyl->SetVisible(true);
}

//returns true when animation is over
bool CylinderRenderer::UpdateAnimation(float dt)
{
	if (cylinderObjects.size() > 0 && animatingNow)
	{
		CylinderObject* rootCyl = cylinderObjects.front();

		bool isFinished = rootCyl->UpdateAnimationRecurs(dt);
		animatingNow = !isFinished;		
	}

	return !animatingNow;
}

void CylinderRenderer::SetShaderParams(const XMMATRIX& _viewMatrix,	const XMMATRIX& _projectionMatrix)
{
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
}

void CylinderRenderer::RenderBlur(D3D* renderer, Camera* camera)
{
	XMMATRIX worldMatrix     = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix(); // Default camera position for orthographic rendering
	XMMATRIX orthoMatrix     = renderer->getOrthoMatrix();   // ortho matrix for 2D rendering
	
														 //1. Render cylinders to the blur texture:
	blurRenderTexture1->setRenderTarget(renderer->getDeviceContext());
	blurRenderTexture1->clearRenderTarget(
		renderer->getDeviceContext(),
		backgroundColour.x,
		backgroundColour.y,
		backgroundColour.z,
		backgroundColour.w
	);
		
	for (CylinderObject* c : cylinderObjects)
	{
		if (c->IsVisible())
		{
			XMFLOAT4 colour = DxColourLerp(
				backgroundColour,
				blurColour,
				c->GetBrightness()
			);

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

void CylinderRenderer::RenderCylinders(D3D* renderer)
{
	for (CylinderObject* c : cylinderObjects)
	{
		if (c->IsVisible())
		{
			XMFLOAT4 colour = DxColourLerp(
				backgroundColour,
				cylinderColour,
				c->GetBrightness()
			);

			c->GetMesh()->sendData(renderer->getDeviceContext());
			mainShader->setShaderParameters(renderer->getDeviceContext(), c->GetTransform(), viewMatrix, projectionMatrix, c->GetTexture(), colour);
			mainShader->render(renderer->getDeviceContext(), c->GetMesh()->getIndexCount());
		}
	}
}

////
// PRIVATE:
////

void CylinderRenderer::CreateCylinderRecurs(Segment* seg, CylinderObject* parentCyl)
{
	CylinderObject* newCylinder = new CylinderObject(*baseCylinder);
	
	newCylinder->Init(seg);

	newCylinder->SetBrightness(
		MyClamp(
		(seg->GetEnergy() / maxEnergy),
			0.f,
			1.f
		)
	);

	if (parentCyl)
	{
		parentCyl->AddChild(newCylinder);
		newCylinder->SetParent(parentCyl);
	}

	cylinderObjects.push_back(newCylinder);

	for (Segment* s : *(seg->GetChildren()))
	{
		CreateCylinderRecurs(s, newCylinder);
	}
}