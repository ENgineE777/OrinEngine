
#include "PhysTriger2D.h"
#include "Root/Root.h"

namespace Oak
{

	ENTITYREG(SceneEntity, PhysTriger2D, "2D/Physics", "PhysTriger2D")

	META_DATA_DESC(PhysTriger2D)
		BASE_SCENE_ENTITY_PROP(PhysTriger2D)
		COLOR_PROP(PhysTriger2D, color, COLOR_YELLOW, "Geometry", "color")
		INT_PROP(PhysTriger2D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(PhysTriger2D, visibleDuringPlay, true, "Physics", "visibleDuringPlay", "Show collision during play")
	META_DATA_DESC_END()

	void PhysTriger2D::Init()
	{
		PhysTriger3D::Init();

		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ | RectMoveXY | RectSizeXY | RectAnchorn;

		transform.size = 100.0f;
		transform.size.z = 20.0f;
	}

	void PhysTriger2D::Play()
	{
		SceneEntity::Play();

		Math::Matrix mat = transform.GetGlobal();
		mat.Pos() += Sprite::ToUnits(transform.size * Math::Vector3(-transform.offset.x + 0.5f, transform.offset.y - 0.5f, 0.0f));

		body.object = this;
		body.body = root.GetPhysScene()->CreateBox(Sprite::ToUnits(transform.size), mat, Math::Matrix(), (PhysObject::BodyType)bodyType, physGroup);
		body.body->SetActive(IsVisible());

		if (bodyType == BodyType::Dynamic || bodyType == BodyType::DynamicCCD)
		{
			body.body->RestrictZAxis();
		}

		body.body->SetUserData(&body);
	}

	void PhysTriger2D::Draw(float dt)
	{
		if (!IsVisible())
		{
			return;
		}

		if (visibleDuringPlay || !GetScene()->IsPlaying())
		{
			Math::Matrix mat = transform.GetGlobal();
			mat.Pos() += Sprite::ToUnits(transform.size * Math::Vector3(-transform.offset.x + 0.5f, transform.offset.y - 0.5f, 0.0f));
			root.render.DebugBox(mat, color, Sprite::ToUnits(transform.size));
		}
	}
}