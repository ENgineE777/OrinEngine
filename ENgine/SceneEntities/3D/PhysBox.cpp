
#include "PhysBox.h"
#include "Root/Root.h"

namespace Oak
{

	CLASSREG(SceneEntity, PhysBox, "PhysBox")

	META_DATA_DESC(PhysBox)
		BASE_SCENE_ENTITY_PROP(PhysBox)
		COLOR_PROP(PhysBox, color, COLOR_YELLOW, "Geometry", "color")
		INT_PROP(PhysBox, phys_group, 1, "Physics", "PhysGroup", "Physical group")
		BOOL_PROP(PhysBox, isStatic, false, "Physics", "Is Static", "Set if object should be satic or dynamic")
	META_DATA_DESC_END()

	void PhysBox::Init()
	{
		transform.transformFlag = MoveXYZ | RotateXYZ | SizeXYZ;

		Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysBox::Draw);
		GetScene()->AddToGroup(this, "PhysBox");
	}

	void PhysBox::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		if (body.body && !isStatic)
		{
			body.body->GetTransform(transform.global);
		}
		else
		{
			transform.BuildMatrices();
		}

		root.render.DebugBox(transform.global, color, transform.size);
	}

	void PhysBox::SetVisible(bool state)
	{
		SceneEntity::SetVisible(state);

		if (body.body)
		{
			body.body->SetActive(state);
		}
	}

	void PhysBox::Play()
	{
		SceneEntity::Play();

		transform.BuildMatrices();

		body.object = this;
		body.body = root.GetPhysScene()->CreateBox(transform.size, transform.global, Math::Matrix(), isStatic ? PhysObject::Static : PhysObject::Dynamic, phys_group);

		Math::Matrix kjljl;
		body.body->GetTransform(kjljl);

		body.body->SetUserData(&body);
	}

	void PhysBox::Release()
	{
		RELEASE(body.body);
		SceneEntity::Release();
	}

	#ifdef OAK_EDITOR
	/*bool PhysBox::CheckSelection(Vector2 ms, Vector3 start, Vector3 dir)
	{
		return Math::IntersectBBoxRay(transform.Pos() - size * 0.5f, transform.Pos() + size * 0.5f, start, dir);
	}*/
	#endif
}