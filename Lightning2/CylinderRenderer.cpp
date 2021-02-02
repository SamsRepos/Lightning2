#include "CylinderRenderer.h"

#include "MyVectorUtil.h"
#include "MyMath.h"

CylinderRenderer::CylinderRenderer(D3D* renderer, HWND hwnd, int _screenWidth, int _screenHeight)
	:
	cylinderMesh(NULL),
	baseCylinder(NULL)
{
	cylinderMesh = new CylinderMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		.5f,
		.5f //diameter of 1, which matches defaylt height
		//leave resolution as default for now
	);

	//Scene objects:
	baseCylinder = new SceneObject(cylinderMesh, NULL, renderer->getWorldMatrix());

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
}

void CylinderRenderer::InitParameters(
	const XMFLOAT4& _blurColour,
	const XMFLOAT4& _blurBackgroundColour,
	const XMFLOAT4& _cylinderColour,
	float _blurExtent,
	float _blurRange
)
{
	blurColour           = _blurColour;
	blurBackgroundColour = _blurBackgroundColour;
	cylinderColour       = _cylinderColour;
	blurExtent           = _blurExtent;
	blurRange            = _blurRange;
}

void CylinderRenderer::Build(std::vector<Segment*>* segments)
{
	cylinderObjects.clear();

	for (Segment* seg : *segments)
	{
		SceneObject newCylinder(*baseCylinder);

		XMFLOAT3 startPosFloat3 = XMFLOAT3(
			seg->GetStartPoint().x,
			seg->GetStartPoint().y,
			seg->GetStartPoint().z
		);
			
		XMFLOAT3 endPosFloat3 = XMFLOAT3(
			seg->GetEndPoint().x,
			seg->GetEndPoint().y,
			seg->GetEndPoint().z
		);

		//position:
		newCylinder.setPosition(XMFLOAT3(startPosFloat3.x, startPosFloat3.y, startPosFloat3.z));

		//scale:
		newCylinder.setScale(
			seg->GetDiameter(),
			seg->GetLength(),
			seg->GetDiameter()
		);

		//direction for roll/pitch/yaw for rotation:
		XMVECTOR startPos = XMLoadFloat3(&startPosFloat3);
		XMVECTOR endPos = XMLoadFloat3(&endPosFloat3);

		XMVECTOR direction = endPos - startPos;

		//direction = XMVector3Normalize(direction);

		float dirX = XMVectorGetX(direction);
		float dirY = XMVectorGetY(direction);
		float dirZ = XMVectorGetZ(direction);

		//yaw value rotates cylinder, around Y axis, towards  new point...
		float yaw = atan(dirX / dirZ);

		//roll value is just used for correction where the branch is pointing downwards:

		float roll = (dirY < 0.f) ? PI : 0.f;

		//pitch down value is determined by tranforming direction coordinates onto the X-axis:
		XMMATRIX axisChanger    = XMMatrixRotationRollPitchYaw(0.f, -yaw, 0.f);
		XMVECTOR transDirection = XMVector3Transform(direction, axisChanger);

		float tdirX = XMVectorGetX(transDirection);
		float tdirY = XMVectorGetY(transDirection);
		float tdirZ = XMVectorGetZ(transDirection);

		float pitch = atan(tdirZ / tdirY);

		//(glitch fix, for if the branch is perfectly vertical)
		if (dirZ == 0.f && dirX == 0.f)
		{
			if (dirY > 0.f)
			{
				yaw = 1.f;
				pitch = roll = 0.f;
			}
			else
			{
				yaw = -1.f;
				pitch = PI;
				roll = 0.f;
			}			
		}

		newCylinder.setRotation(pitch, yaw, roll);

		newCylinder.updateObjectMatrix();

		cylinderObjects.push_back(newCylinder);
	}

	cylindersToRender = cylinderObjects.size();
}

void CylinderRenderer::SetShaderParams(
	const XMMATRIX& _viewMatrix,
	const XMMATRIX& _projectionMatrix	
)
{
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
}


void CylinderRenderer::RenderBlur(D3D* renderer, Camera* camera)
{
	XMMATRIX worldMatrix = renderer->getWorldMatrix();
	XMMATRIX orthoViewMatrix = camera->getOrthoViewMatrix(); // Default camera position for orthographic rendering
	XMMATRIX orthoMatrix = renderer->getOrthoMatrix();   // ortho matrix for 2D rendering
	
														 //1. Render cylinders to the blur texture:
	blurRenderTexture1->setRenderTarget(renderer->getDeviceContext());
	blurRenderTexture1->clearRenderTarget(
		renderer->getDeviceContext(),
		blurBackgroundColour.x,
		blurBackgroundColour.y,
		blurBackgroundColour.z,
		blurBackgroundColour.w
	);

	for (auto c : cylinderObjects)
	{
		c.getMesh()->sendData(renderer->getDeviceContext());
		mainShader->setShaderParameters(renderer->getDeviceContext(), c.getObjectMatrix(), viewMatrix, projectionMatrix, c.getTexture(), blurColour);
		mainShader->render(renderer->getDeviceContext(), c.getMesh()->getIndexCount());
	}

	//2. Render blur texture for blur pass:
	blurRenderTexture2->setRenderTarget(renderer->getDeviceContext());
	blurRenderTexture2->clearRenderTarget(
		renderer->getDeviceContext(),
		blurBackgroundColour.x,
		blurBackgroundColour.y,
		blurBackgroundColour.z,
		blurBackgroundColour.w
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

	blurShader->updateGaussianBlurParameters(
		renderer->getDeviceContext(),
		blurExtent,
		blurRange
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
	for (auto c : cylinderObjects)
	{
		c.getMesh()->sendData(renderer->getDeviceContext());
		mainShader->setShaderParameters(renderer->getDeviceContext(), c.getObjectMatrix(), viewMatrix, projectionMatrix, c.getTexture(), cylinderColour);
		mainShader->render(renderer->getDeviceContext(), c.getMesh()->getIndexCount());
	}
}

//
//void TreeObject::guiInspectCylinderRotation(size_t currentSeg)
//{
//	
//	Segment* seg = tree->getSegments()[currentSeg];
//	SceneObject* cyl = cylinders[currentSeg];
//
//	ImGui::Text("INSPECTING CYLINDER ROTATION #%d:", currentSeg);
//	
//	//direction for roll/pitch/yaw for rotation:
//	XMVECTOR startPos = XMLoadFloat3(&seg->startpt);
//	XMVECTOR endPos = XMLoadFloat3(&seg->endpt);
//
//	XMVECTOR direction = endPos - startPos;
//
//	//direction = XMVector3Normalize(direction);
//	float dirX = XMVectorGetX(direction);
//	float dirY = XMVectorGetY(direction);
//	float dirZ = XMVectorGetZ(direction);
//
//	ImGui::Text("Dir X: %f Y: %f Z: %f", dirX, dirY, dirZ);
//
//	ImGui::Text("Pitch: %f", cyl->getRotation().x);
//	ImGui::Text("Yaw: %f", cyl->getRotation().y);
//	ImGui::Text("Roll: %f", cyl->getRotation().z);
//	
//	
//	
//}