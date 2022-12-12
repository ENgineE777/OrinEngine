
#include "Math.h"

namespace Oak::Math
{
	bool Vector3::IsEqual(const Vector3& v)
	{
		return Math::IsEqual(x, v.x) && Math::IsEqual(y, v.y) && Math::IsEqual(z, v.z);
	}
}