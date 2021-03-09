#include "DiameterThinner.h"

#include "Maths/MyClamp.h"
#include "DefaultParameters.h"

void DiameterThinner::InitParameters(
	float _scale
)
{
	scale = MyClamp(
		_scale,
		DTHIN_MIN_SCALEDOWN,
		DTHIN_MAX_SCALEDOWN
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