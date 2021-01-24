#include "CylinderRenderer.h"


// from 3rd year project - to adapt to this next time...

TreeObject::TreeObject(Tree* _tree, SceneObject _cylinder)
	:
	tree(_tree),
	baseCylinder(_cylinder)
{
	setPosition(0.f, 0.f, 0.f);
}

TreeObject::~TreeObject()
{
}

void TreeObject::init()
{
	cylinders.clear();

	std::vector<Segment*> segs = tree->getSegments();
	for (Segment* s : segs)
	{
		//SceneObject* newCylinder = new SceneObject(cylinderObject->getMesh(), cylinderObject->getTexture(), renderer->getWorldMatrix());

		SceneObject newCylinder(baseCylinder);

		XMFLOAT3 startPosFloat3 = s->startpt;
		XMFLOAT3 endPosFloat3 = s->endpt;

		//position:
		newCylinder.setPosition(XMFLOAT3(startPosFloat3.x + position.x, startPosFloat3.y + position.y, startPosFloat3.z + position.z));

		//scale:
		float branchThinner = 1.f - (float(s->treelevel) / float(tree->getHighestLevel()));
		newCylinder.setScale(s->magnitude * branchThinner, s->magnitude, s->magnitude * branchThinner);

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
#define PI 3.14159265359f
		float roll = (dirY < 0.f) ? PI : 0.f;

		//pitch down value is determined by tranforming direction coordinates onto the X-axis:
		XMMATRIX axisChanger = XMMatrixRotationRollPitchYaw(0.f, -yaw, 0.f);
		XMVECTOR transDirection = XMVector3Transform(direction, axisChanger);

		float tdirX = XMVectorGetX(transDirection);
		float tdirY = XMVectorGetY(transDirection);
		float tdirZ = XMVectorGetZ(transDirection);

		float pitch = atan(tdirZ / tdirY);

		//(glitch fix, for if the branch points vertically upwards)
		if (dirZ == 0.f && dirX == 0.f)
		{
			yaw = 1.f;
			pitch = roll = 0.f;
		}

		newCylinder.setRotation(pitch, yaw, roll);

		newCylinder.updateObjectMatrix();

		cylinders.push_back(newCylinder);
	}

	cylinderCount = cylinders.size();

}

void TreeObject::updateCylinders()
{
	assert(cylinderCount == tree->getSegments().size());

	for (size_t i = 0; i < cylinderCount; i++)
	{
		Segment* seg = tree->getSegments()[i];
		SceneObject* cyl = &(cylinders[i]);

		XMFLOAT3 startPosFloat3 = seg->startpt;
		XMFLOAT3 endPosFloat3 = seg->endpt;

		//position:
		cyl->setPosition(XMFLOAT3(startPosFloat3.x + position.x, startPosFloat3.y + position.y, startPosFloat3.z + position.z));

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
#define PI 3.14159265359f
		float roll = (dirY < 0.f) ? PI : 0.f;

		//pitch down value is determined by tranforming direction coordinates onto the X-axis:
		XMMATRIX axisChanger = XMMatrixRotationRollPitchYaw(0.f, -yaw, 0.f);
		XMVECTOR transDirection = XMVector3Transform(direction, axisChanger);

		float tdirX = XMVectorGetX(transDirection);
		float tdirY = XMVectorGetY(transDirection);
		float tdirZ = XMVectorGetZ(transDirection);

		float pitch = atan(tdirZ / tdirY);

		//(glitch fix, for if the branch points vertically upwards)
		if (dirZ == 0.f && dirX == 0.f)
		{
			yaw = 1.f;
			pitch = roll = 0.f;
		}

		cyl->setRotation(pitch, yaw, roll);
		cyl->updateObjectMatrix();
	}
}

void TreeObject::render(D3D* renderer, LightShader* shader, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, Light* light)
{
	for (auto c : cylinders)
	{
		c.getMesh()->sendData(renderer->getDeviceContext());
		shader->setShaderParameters(renderer->getDeviceContext(), c.getObjectMatrix(), viewMatrix, projectionMatrix, c.getTexture(), light);
		shader->render(renderer->getDeviceContext(), c.getMesh()->getIndexCount());
	}
}

void TreeObject::render(D3D* renderer, LightShader* shader, const XMMATRIX &viewMatrix, const XMMATRIX &projectionMatrix, Light* light, size_t currentSeg, ID3D11ShaderResourceView* highlightTexture)
{
	for (size_t i = 0; i < cylinderCount; i++)
	{
		cylinders[i].getMesh()->sendData(renderer->getDeviceContext());

		shader->setShaderParameters(renderer->getDeviceContext(), cylinders[i].getObjectMatrix(), viewMatrix, projectionMatrix,
			(i == (currentSeg % cylinders.size())) ? highlightTexture : cylinders[i].getTexture(),
			light);

		shader->render(renderer->getDeviceContext(), cylinders[i].getMesh()->getIndexCount());

		if (i == (currentSeg % cylinderCount))
		{
			break;
		}
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