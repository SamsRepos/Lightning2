#include "AnimSegment.h"

#include "Maths/MyClamp.h"

AnimSegment::AnimSegment(Segment* seg)
{
	visible      = true;
	animatingNow = false;

	startPoint      = seg->GetStartPoint();
	currentEndPoint = seg->GetEndPoint();
	fixedEndPoint   = seg->GetEndPoint();
	currentLength   = seg->GetLength();
	fixedLength     = seg->GetLength();
	direction       = seg->GetDirection().Normalised();
	
	diameter = seg->GetDiameter();
	velocity = seg->GetVelocity();
	energy   = seg->GetEnergy();
	status   = seg->GetStatus();
}

void AnimSegment::InitAnimation()
{
	visible         = false;
	animatingNow    = true;
	currentEndPoint = startPoint;
	currentLength   = 0.f;
}

// returns true while animating
bool AnimSegment::UpdateAnimationRecurs(float dt)
{
	float deltaTimeUsed = 0.f;

	if (animatingNow)
	{
		// Growth of this segment:
		float deltaLength = velocity * dt;

		// The normal case:
		if ((currentLength + deltaLength) < fixedLength)
		{			
			currentLength += deltaLength;
			currentEndPoint = currentEndPoint + (direction * deltaLength);
		}
		else // Preventing an overshoot:
		{
			// Delta time used for this segment is now computed for next recursion
			// A possible lightning slow-down due to the frame rate is undesirable...
			// ...so for the next segment down, we're going now! (next block below)
			// but, reducing deltaTime so we don't overshoot:
			deltaLength = (fixedLength - currentLength);
			deltaTimeUsed = (deltaLength / velocity);

			animatingNow = false;

			currentLength   = fixedLength;
			currentEndPoint = fixedEndPoint;

			//If there are no children, animating this branch is over:
			if (children.empty())
			{
				return false;
			}
			else
			{
				for (AnimSegment* child : children)
				{
					child->SetVisible(true);
				}
			}
		}		
	}

	if (!animatingNow)
	{
		bool res = true;

		float nextDeltaTime = dt - deltaTimeUsed;
		nextDeltaTime = std::max(0.f, nextDeltaTime);

		for (AnimSegment* child : children)
		{
			res = child->UpdateAnimationRecurs(nextDeltaTime) || res;
		}

		return res;
	}

	return true;
}