
#include "PhysEntity2D.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, PhysEntity2D, "2D/Physics", "PhysEntity2D")

	META_DATA_DESC(PhysEntity2D)
		BASE_SCENE_ENTITY_PROP(PhysEntity2D)
		COLOR_PROP(PhysEntity2D, color, COLOR_YELLOW, "Geometry", "color")
		ENUM_PROP(PhysEntity2D, bodyShape, 0, "Physics", "Body shape", "Body shape")
			ENUM_ELEM("Box", 0)
			ENUM_ELEM("Sphere", 1)
			ENUM_ELEM("Capsule", 2)
		ENUM_END
		ENUM_PROP(PhysEntity2D, bodyType, 0, "Physics", "Body type", "Body type")
			ENUM_ELEM("Static", 0)
			ENUM_ELEM("Dynamic", 1)
			ENUM_ELEM("DynamicCCD", 2)
			ENUM_ELEM("Kinematic", 3)
		ENUM_END
		INT_PROP(PhysEntity2D, physGroup, 1, "Physics", "group", "Physical group")
		BOOL_PROP(PhysEntity2D, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
		BOOL_PROP(PhysEntity2D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
	META_DATA_DESC_END()

	void PhysEntity2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ | RectMoveXY | RectSizeXY;

		transform.size = 50.0f;
		transform.size.z = 50.0f;

		Tasks(false)->AddTask(100, this, (Object::Delegate)&PhysEntity2D::Draw);
		GetScene()->AddToGroup(this, "PhysBox2D");
	}

	void PhysEntity2D::Play()
	{
		SceneEntity::Play();

		Math::Matrix mat = transform.GetGlobal();
		auto scale = mat.GetScale();

		auto size = Sprite::ToUnits(transform.size) * scale;

		mat.Pos() += size * Sprite::ToUnits(Math::Vector3(-transform.offset.x + 0.5f, transform.offset.y - 0.5f, 0.0f));

		body.object = this;

		switch (bodyShape)
		{
		case Orin::PhysEntityBase::BodyShape::Box:
			body.body = root.GetPhysScene()->CreateBox(size, mat, Math::Matrix(), (PhysObject::BodyType)bodyType, physGroup);
			break;
		case Orin::PhysEntityBase::BodyShape::Sphere:
			body.body = root.GetPhysScene()->CreateSphere(size.x * 0.5f, mat, Math::Matrix(), (PhysObject::BodyType)bodyType, physGroup);
			break;
		case Orin::PhysEntityBase::BodyShape::Capsule:			
			body.body = root.GetPhysScene()->CreateCapsule(size.y * 0.5f, size.x - size.y, mat, Math::Matrix(), (PhysObject::BodyType)bodyType, physGroup);
			break;
		default:
			break;
		}

		body.body->SetActive(IsVisible());

		if (bodyType == BodyType::Dynamic || bodyType == BodyType::DynamicCCD)
		{
			body.body->RestrictZAxis();
		}

		body.body->SetUserData(&body);
	}

	void PhysEntity2D::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		if (body.body && (bodyType == BodyType::Dynamic || bodyType == BodyType::DynamicCCD))
		{
			Math::Matrix mat;
			body.body->GetTransform(mat);

			if (affectOnParent && parent)
			{
				auto& parentTrans = parent->GetTransform();
				parentTrans.SetGlobal(mat);
			}
			else
			{
				transform.SetGlobal(mat);
			}
		}

		if (visibleDuringPlay || !GetScene()->IsPlaying())
		{
			Math::Matrix mat = transform.GetGlobal();
			
			if (bodyShape == Orin::PhysEntityBase::BodyShape::Sphere)
			{
				transform.size.y = transform.size.x;
			}
			else
			if (bodyShape == Orin::PhysEntityBase::BodyShape::Capsule)
			{
				if (transform.size.x < transform.size.y + 0.01f)
				{
					transform.size.x = transform.size.y + 0.01f;
				}
			}

			auto size = Sprite::ToUnits(transform.size);

			switch (bodyShape)
			{
			case Orin::PhysEntityBase::BodyShape::Box:
				root.render.DebugBox(mat, color, size);
				break;
			case Orin::PhysEntityBase::BodyShape::Sphere:
				root.render.DebugSphere(mat.Pos(), color, size.x * 0.5f);
				break;
			case Orin::PhysEntityBase::BodyShape::Capsule:

				float height = size.x - size.y;

				Math::Matrix mat = transform.GetGlobal();
				const Math::Vector3 top = mat * Math::Vector3(height * 0.5f, 0.0f, 0.0f);
				const Math::Vector3 bottom = mat * Math::Vector3(-height * 0.5f, 0.0f, 0.0f);

				//root.render.DebugBox(mat, color, { height, size.y, size.z });
				root.render.DebugSphere(top, color, size.y * 0.5f);
				root.render.DebugSphere(bottom, color, size.y * 0.5f);

				break;
			}
		}
	}
}