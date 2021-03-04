#include "CylinderObject.h"

#include "MyClamp.h"
#include "MyLerp.h"

CylinderObject::CylinderObject(
	BaseMesh* _mesh,
	ID3D11ShaderResourceView* _texture,
	const XMMATRIX &_worldOriginTransform
)
	:
	SceneObject::SceneObject(_mesh, _texture, _worldOriginTransform)
{
	visible = true;
}

void CylinderObject::Init(Segment* seg)
{
	diameter = seg->GetDiameter();
	length =   seg->GetLength();
	velocity = seg->GetVelocity();

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
	SetPosition(XMFLOAT3(startPosFloat3.x, startPosFloat3.y, startPosFloat3.z));

	//scale:
	SetScale(diameter, length, diameter);

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

	SetRotation(pitch, yaw, roll);

	BuildTransform();
}

void CylinderObject::InitAnimation()
{	
	t = 0.f;
	SetScale(0);
	BuildTransform();
	finishedAnimating = false;
}

bool CylinderObject::UpdateAnimationRecurs(float deltaTime)
{
	if (finishedAnimating)
	{
		bool res = false;

		for (CylinderObject* child : children)
		{
			res = child->UpdateAnimationRecurs(deltaTime) && res;
		}
		return res;
	}

	// Growth:
	float deltaLength = velocity * deltaTime;
	t += deltaLength / length;
	t = MyClamp(t, 0.f, 1.f);

	SetScale(
		XMFLOAT3(			
			diameter,
			MyLerp(0.f, length, t),
			diameter
		)
	);
	BuildTransform();
	
	if (t >= 1.f)
	{
		finishedAnimating = true;
		SetScale(diameter, length, diameter); // ensuring any slight t>1.f is fixed

		if (children.empty())
		{
			return true;
		}
		else
		{
			for (CylinderObject* child : children)
			{
				child->SetVisible(true);
			}
		}		
	}

	return false;
}