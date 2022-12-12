
#include "Math.h"

namespace Oak::Math
{
	bool Vector4::IsEqual(const Vector4& v)
	{
		return Math::IsEqual(x, v.x) && Math::IsEqual(y, v.y) && Math::IsEqual(z, v.z) && Math::IsEqual(w, v.w);
	}
}