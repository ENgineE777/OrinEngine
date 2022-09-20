#include "ScriptEntity2D.h"
#include "Root/Root.h"

namespace Oak
{
	void ScriptEntity2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ;

		transform.size = 100.0f;
	}
}