
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

	void TestEntity3D::Draw(float dt)
	{
		transform.BuildMatrices();
		root.render.DebugBox(transform.global, COLOR_GREEN, 1.0f);
	}
}
