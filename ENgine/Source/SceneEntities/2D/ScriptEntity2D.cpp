#include "ScriptEntity2D.h"
#include "Root/Root.h"

namespace Oak
{
	void ScriptEntity2D::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
		transform.transformFlag = MoveXYZ;

		transform.size = 100.0f;
	}
}