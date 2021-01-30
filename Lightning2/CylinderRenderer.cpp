#include "CylinderRenderer.h"

#include "MyVectorUtil.h"
#include "MyMath.h"

CylinderRenderer::CylinderRenderer()
	:
	cylinderMesh(NULL),
	baseCylinder(NULL)
{
	
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

void CylinderRenderer::Init(D3D* renderer, HWND hwnd, ID3D11ShaderResourceView* texture)
{
	cylinderMesh = new CylinderMesh(
		renderer->getDevice(),
		renderer->getDeviceContext(),
		.5f,
		.5f //diameter of 1, which matches defaylt height
		//leave resolution as default for now
	);

	//SCENEOBJECTS:
	baseCylinder = new SceneObject(cylinderMesh, texture, renderer->getWorldMatrix());

	//Shader:
	shader = new CylinderShader(renderer->getDevice(), hwnd);
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
	const XMMATRIX& _projectionMatrix,
	const XMFLOAT4& _colour
)
{
	viewMatrix       = _viewMatrix;
	projectionMatrix = _projectionMatrix;
	colour           = _colour;
}


void CylinderRenderer::Render(D3D* renderer)
{
	for (auto c : cylinderObjects)
	{
		c.getMesh()->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), c.getObjectMatrix(), viewMatrix, projectionMatrix, c.getTexture(), colour);
		shader->render(renderer->getDeviceContext(), c.getMesh()->getIndexCount());
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