#include "DiameterThinner.h"

#include "Maths/MyClamp.h"
#include "DefaultParameters.h"

void DiameterThinner::InitParameters(
	float _scale
)
{
	scale = MyClamp(
		_scale,
		DT_MIN_SCALEDOWN,
		DT_MAX_SCALEDOWN
	);
}

void DiameterThinner::Run()
{
	for (Segment* segment : *segments)
	{
		segment->SetDiameter(
			segment->GetDiameter() * scale
		);
	}
}