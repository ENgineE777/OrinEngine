
#include "TestEntity3D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, TestEntity3D, "TestEntity3D")

	META_DATA_DESC(TestEntity3D)
		BASE_SCENE_ENTITY_PROP(TestEntity3D)
	META_DATA_DESC_END()

	void TestEntity3D::Init()
	{
		Tasks(false)->AddTask(0, this, (Object::Delegate)&TestEntity3D::Draw);
	}

	Transform* TestEntity3D::GetTransform()
	{
		return &tranform;
	}

	void TestEntity3D::Draw(float dt)
	{
		root.render.DebugSphere(tranform.local.Pos(), COLOR_GREEN, 1.0f);
	}
}
