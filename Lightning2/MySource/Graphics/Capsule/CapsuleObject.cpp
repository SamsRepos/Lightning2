#include "CapsuleObject.h"

#include "Maths/MyMath.h"

////
// PUBLIC:
////

CapsuleObject::CapsuleObject(
	BaseMesh* _cylinderMesh,
	BaseMesh* _domeMesh,
	ID3D11ShaderResourceView* _texture,
	const XMMATRIX &_worldOriginTransform
)
	:
	SceneObject::SceneObject(NULL, _texture, _worldOriginTransform),
	cylinderMesh(_cylinderMesh),
	domeMesh(_domeMesh)
{	
}

void CapsuleObject::Init(AnimSegment* _animSeg)
{	
	animSeg = _animSeg;

	XMFLOAT3 startPosFloat3 = XMFLOAT3(
		animSeg->GetStartPoint().x,
		animSeg->GetStartPoint().y,
		animSeg->GetStartPoint().z
	);

	XMFLOAT3 endPosFloat3 = XMFLOAT3(
		animSeg->GetFixedEndPoint().x,
		animSeg->GetFixedEndPoint().y,
		animSeg->GetFixedEndPoint().z
	);

	//position:
	SetPosition(XMFLOAT3(startPosFloat3.x, startPosFloat3.y, startPosFloat3.z));

	//scale:
	SetScale(
		animSeg->GetDiameter(),
		animSeg->GetFixedLength(),
		animSeg->GetDiameter()
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
	XMMATRIX axisChanger = XMMatrixRotationRollPitchYaw(0.f, -yaw, 0.f);
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

	SetRotation(pitch, yaw, roll);

	BuildAllTransforms();
}

void CapsuleObject::UpdateFromAnimation()
{
	SetScale(
		animSeg->GetDiameter(),
		animSeg->GetCurrentLength(),
		animSeg->GetDiameter()
	);
	
	BuildAllTransforms();
}

void CapsuleObject::Render(
	MyBaseShader* shader,
	D3D* renderer,
	const XMMATRIX& viewMatrix,
	const XMMATRIX& projectionMatrix,
	const XMFLOAT4& colour
)
{
	// CYLINDER:
	cylinderMesh->sendData(renderer->getDeviceContext());
	
	shader->SetShaderParameters(
		renderer->getDeviceContext(),
		cylinderTransform,
		viewMatrix,
		projectionMatrix,
		GetTexture()
	);

	shader->render(
		renderer->getDeviceContext(),
		cylinderMesh->getIndexCount()
	);

	// FIRST DOME:
	domeMesh->sendData(renderer->getDeviceContext());

	shader->SetShaderParameters(
		renderer->getDeviceContext(),
		domeTransform1,
		viewMatrix,
		projectionMatrix,
		GetTexture()
	);

	shader->render(
		renderer->getDeviceContext(),
		domeMesh->getIndexCount()
	);

	// SECOND DOME:
	
	shader->SetShaderParameters(
		renderer->getDeviceContext(),
		domeTransform2,
		viewMatrix,
		projectionMatrix,
		GetTexture()
	);
	
	shader->render(
		renderer->getDeviceContext(),
		domeMesh->getIndexCount()
	);
}

void CapsuleObject::BuildAllTransforms()
{
	BuildTransform();
	cylinderTransform = GetTransform();
	BuildDomeTransforms();
}

////
// PRIVATE:
////

void CapsuleObject::BuildDomeTransforms()
{
	//TRANSLATION:
	//Dome 1 is at the start of the cylinder::
	XMMATRIX translateMatrix1 = XMMatrixTranslation(
		GetPosition().x,
		GetPosition().y,
		GetPosition().z
	);
	domeTransform1 = XMMatrixMultiply(translateMatrix1, GetWorldOriginTransform());
	//Dome 2 is at the end of the cylinder:
	XMMATRIX translateMatrix2 = XMMatrixTranslation(
		animSeg->GetCurrentEndPoint().x,
		animSeg->GetCurrentEndPoint().y,
		animSeg->GetCurrentEndPoint().z
	);
	domeTransform2 = XMMatrixMultiply(translateMatrix2, GetWorldOriginTransform());

	//ROTATION:
	XMMATRIX cylinderRotationMatrix = XMMatrixRotationRollPitchYaw(
		GetRotation().x,
		GetRotation().y,
		GetRotation().z
	);
	domeTransform1 = XMMatrixMultiply(cylinderRotationMatrix, domeTransform1);
	domeTransform2 = XMMatrixMultiply(cylinderRotationMatrix, domeTransform2);

	//scale:
	float domeScale = animSeg->GetDiameter();

	//for the top dome, flipping the Y direction to invert dome obviously
	//and flipping one other direction, so that polygons face outwards again
	XMMATRIX scaleMatrix1 = XMMatrixScaling(-domeScale, -domeScale, domeScale);
	domeTransform1 = XMMatrixMultiply(scaleMatrix1, domeTransform1);

	XMMATRIX scaleMatrix2 = XMMatrixScaling(domeScale, domeScale, domeScale);
	domeTransform2 = XMMatrixMultiply(scaleMatrix2, domeTransform2);
};