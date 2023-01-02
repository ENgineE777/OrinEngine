
#include "Math.h"

namespace Orin::Math
{
	bool Vector2::IsEqual(const Vector2& v)
	{
		return Math::IsEqual(x, v.x) && Math::IsEqual(y, v.y);
	}
}