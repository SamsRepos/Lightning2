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
	diameter      = seg->GetDiameter();
	currentLength = seg->GetLength();
	fixedLength   = seg->GetLength();
	velocity      = seg->GetVelocity();

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
	SetScale(diameter, fixedLength, diameter);

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
	currentLength = 0.f;
	SetScale(0);
	BuildTransform();
	finishedAnimating = false;
	SetVisible(false);
}

bool CylinderObject::UpdateAnimationRecurs(float deltaTime)
{

	float deltaTimeTaken = 0.f;
	
	if (!finishedAnimating) {
		// Growth:
		float deltaLength = velocity * deltaTime;
		
		// If there is going to be an overshoot...
		if ((currentLength + deltaLength) > fixedLength)
		{
			// just make deltaLength the amount remaining till the end of the segment
			deltaLength = (fixedLength - currentLength);
		}

		currentLength += deltaLength;

		SetScale(
			XMFLOAT3(
				diameter,
				currentLength,
				diameter
			)
		);
		BuildTransform();

		if (currentLength >= fixedLength)
		{
			finishedAnimating = true;
			SetScale(diameter, fixedLength, diameter); // ensuring any slight t>1.f is fixed

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

			// deltaTimeTaken is calculated now
			// a possible lightning slow-down due to the frame rate is undesirable...
			// ...so for the next segment down, we're going now! (next block below)
			// but, reducing deltaTime so we don't overshoot...

			deltaTimeTaken = (deltaLength / velocity);

			// This is still buggy
			to do:
			also implement this in the line renderer... correctly tho!
		}
	}

	if (finishedAnimating)
	{
		bool res = false;

		for (CylinderObject* child : children)
		{
			res = child->UpdateAnimationRecurs(deltaTime) && res;
		}
		return res;
	}


	return false;
}