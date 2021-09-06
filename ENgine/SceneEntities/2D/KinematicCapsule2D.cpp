
#include "KinematicCapsule2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, KinematicCapsule2D, "KinematicCapsule2D")

	META_DATA_DESC(KinematicCapsule2D)
		BASE_SCENE_ENTITY_PROP(KinematicCapsule2D)
		INT_PROP(KinematicCapsule2D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(KinematicCapsule2D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
	META_DATA_DESC_END()

	void KinematicCapsule2D::Init()
	{
		transform.transformFlag = MoveXYZ;

		Tasks(false)->AddTask(0, this, (Object::Delegate)&KinematicCapsule2D::EditorDraw);
	}

	void KinematicCapsule2D::OnVisiblityChange(bool state)
	{
		if (controller)
		{
			controller->SetActive(state);
		}
	}

	void KinematicCapsule2D::Play()
	{
		PhysControllerDesc desc;
		desc.height = 1.0f;

		transform.BuildMatrices();

		auto size = Math::Vector2(100.0f, 150.0f);
		desc.radius = fminf(size.x, size.y) * Sprite::pixelsPerUnitInvert * 0.5f * 0.65f;
		desc.upVector.Set(0.0f, 0.0f, 1.0f);
		desc.pos = transform.global.Pos() * Sprite::pixelsPerUnitInvert;

		controller = GetRoot()->GetPhysScene()->CreateController(desc, 2);
		controller->RestrictZAxis();
		controller->SetActive(IsVisible());

		bodyData.object = this;
		bodyData.controller = controller;

		controller->SetUserData(&bodyData);
	}

	void KinematicCapsule2D::Move(Math::Vector2 dir, uint32_t ignoreGroup)
	{
		controller->Move(Math::Vector3(dir.x, dir.y, 0.0f) * Sprite::pixelsPerUnitInvert * root.GetDeltaTime(), physGroup, ignoreGroup);

		Math::Vector3 pos;
		controller->GetPosition(pos);

		transform.global.Pos() *= Sprite::pixelsPerUnit;

		if (affectOnParent && parent)
		{
			auto& parentTrans = parent->GetTransform();
			parentTrans.position = transform.global.Pos();
		}
	}

	void KinematicCapsule2D::EditorDraw(float dt)
	{
		if (IsVisible())
		{

		}
	}
}