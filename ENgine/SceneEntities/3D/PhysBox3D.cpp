
#include "PhysBox3D.h"
#include "Root/Root.h"

namespace Oak
{

	CLASSREG(SceneEntity, PhysBox3D, "PhysBox3D")

	META_DATA_DESC(PhysBox3D)
		BASE_SCENE_ENTITY_PROP(PhysBox3D)
		COLOR_PROP(PhysBox3D, color, COLOR_YELLOW, "Geometry", "color")
		ENUM_PROP(PhysBox3D, bodyType, 0, "Physics", "Body type", "Body type")
			ENUM_ELEM("Static", 0)
			ENUM_ELEM("Dynamic", 1)
			ENUM_ELEM("DynamicCCD", 2)
			ENUM_ELEM("Kinematic", 3)
			ENUM_ELEM("Trigger", 4)
		ENUM_END
		INT_PROP(PhysBox3D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(PhysBox3D, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
		BOOL_PROP(PhysBox3D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
	META_DATA_DESC_END()

	void PhysBox3D::Init()
	{
		transform.transformFlag = MoveXYZ | RotateXYZ | SizeXYZ;

		Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysBox3D::Draw);
		GetScene()->AddToGroup(this, "PhysBox3D");
	}

	void PhysBox3D::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		if (body.body && (bodyType == BodyType::Dynamic || bodyType == BodyType::DynamicCCD))
		{
			body.body->GetTransform(transform.global);

			if (affectOnParent && parent)
			{
				auto& parentTrans = parent->GetTransform();
				parentTrans.position = transform.global.Pos();
				parentTrans.rotation = transform.global.GetRotation() / Math::Radian;
			}
		}
		else
		{
			transform.BuildMatrices();
		}

		if (visibleDuringPlay || !GetScene()->IsPlaying())
		{
			root.render.DebugBox(transform.global, color, transform.size);
		}
	}

	void PhysBox3D::OnVisiblityChange(bool state)
	{
		if (body.body)
		{
			body.body->SetActive(state);
		}
	}

	void PhysBox3D::Play()
	{
		SceneEntity::Play();

		transform.BuildMatrices();

		body.object = this;
		body.body = root.GetPhysScene()->CreateBox(transform.size, transform.global, Math::Matrix(), (PhysObject::BodyType)bodyType, physGroup);
		body.body->SetActive(IsVisible());

		body.body->SetUserData(&body);
	}

	void PhysBox3D::Release()
	{
		RELEASE(body.body);
		SceneEntity::Release();
	}

	#ifdef OAK_EDITOR
	bool PhysBox3D::CheckSelection(Math::Vector2 ms, Math::Vector3 start, Math::Vector3 dir)
	{
		return Math::IntersectBBoxRay(transform.global.Pos() - transform.size * 0.5f, transform.global.Pos() + transform.size * 0.5f, start, dir);
	}
	#endif
}