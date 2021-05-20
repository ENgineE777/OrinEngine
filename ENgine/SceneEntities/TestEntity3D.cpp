
#include "TestEntity3D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, TestEntity3D, "TestEntity3D")

	META_DATA_DESC(TestEntity3D)
		BASE_SCENE_ENTITY_PROP(TestEntity3D)
		FLOAT_PROP(TestEntity3D, transform.position.x, 0.0f, "Transform", "x", "X coordinate of an object")
		FLOAT_PROP(TestEntity3D, transform.position.y, 0.0f, "Transform", "y", "Y coordinate of an object")
		FLOAT_PROP(TestEntity3D, transform.position.z, 0.0f, "Transform", "z", "X coordinate of an object")

		FLOAT_PROP(TestEntity3D, transform.rotation.x, 0.0f, "Transform", "rotX", "X coordinate of an object")
		FLOAT_PROP(TestEntity3D, transform.rotation.y, 0.0f, "Transform", "rotY", "Y coordinate of an object")
		FLOAT_PROP(TestEntity3D, transform.rotation.z, 0.0f, "Transform", "rotZ", "X coordinate of an object")

		FLOAT_PROP(TestEntity3D, transform.scale.x, 1.0f, "Transform", "scaleX", "X coordinate of an object")
		FLOAT_PROP(TestEntity3D, transform.scale.y, 1.0f, "Transform", "scaleY", "Y coordinate of an object")
		FLOAT_PROP(TestEntity3D, transform.scale.z, 1.0f, "Transform", "scaleZ", "X coordinate of an object")

	META_DATA_DESC_END()

	void TestEntity3D::Init()
	{
		Tasks(false)->AddTask(0, this, (Object::Delegate)&TestEntity3D::Draw);
	}

	Transform* TestEntity3D::GetTransform()
	{
		return &transform;
	}

	void TestEntity3D::Draw(float dt)
	{
		transform.BuildMatrices();
		root.render.DebugBox(transform.global, COLOR_GREEN, 1.0f);
	}
}
