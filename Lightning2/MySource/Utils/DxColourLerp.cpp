#include "DxColourLerp.h"

#include "Maths/MyLerp.h"

XMFLOAT4 DxColourLerp(const XMFLOAT4& a, const XMFLOAT4& b, float t)
{
	return XMFLOAT4(
		MyLerp(a.x, b.x, t),
		MyLerp(a.y, b.y, t),
		MyLerp(a.z, b.z, t),
		MyLerp(a.w, b.w, t)
	);
}
