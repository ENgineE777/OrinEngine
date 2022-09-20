
#include "PhysBox2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, PhysBox2D, "PhysBox2D")

	META_DATA_DESC(PhysBox2D)
		BASE_SCENE_ENTITY_PROP(PhysBox2D)
		COLOR_PROP(PhysBox2D, color, COLOR_YELLOW, "Geometry", "color")
		ENUM_PROP(PhysBox2D, bodyType, 0, "Physics", "Body type", "Body type")
			ENUM_ELEM("Static", 0)
			ENUM_ELEM("Dynamic", 1)
			ENUM_ELEM("DynamicCCD", 2)
			ENUM_ELEM("Kinematic", 3)
			ENUM_ELEM("Trigger", 4)
		ENUM_END
		INT_PROP(PhysBox2D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(PhysBox2D, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
		BOOL_PROP(PhysBox2D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
	META_DATA_DESC_END()

	void PhysBox2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ | RectMoveXY | RectSizeXY;

		transform.size = 100.0f;
		transform.size.z = 20.0f;

		Tasks(false)->AddTask(100, this, (Object::Delegate)& PhysBox2D::Draw);
		GetScene()->AddToGroup(this, "PhysBox2D");
	}

	void PhysBox2D::Play()
	{
		SceneEntity::Play();

		Math::Matrix mat = transform.GetGlobal();

		body.object = this;
		body.body = root.GetPhysScene()->CreateBox(Sprite::ToUnits(transform.size), mat, Math::Matrix(), (PhysObject::BodyType)bodyType, physGroup);
		body.body->SetActive(IsVisible());

		if (bodyType == BodyType::Dynamic || bodyType == BodyType::DynamicCCD)
		{
			body.body->RestrictZAxis();
		}

		body.body->SetUserData(&body);
	}

	void PhysBox2D::Draw(float dt)
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
				parentTrans.position = mat.Pos();
				parentTrans.rotation = mat.GetRotation() / Math::Radian;
			}
			else
			{
				transform.SetGlobal(mat);
			}
		}

		if (visibleDuringPlay || !GetScene()->IsPlaying())
		{
			Math::Matrix mat = transform.GetGlobal();
			root.render.DebugBox(mat, color, Sprite::ToUnits(transform.size));
		}
	}
}