
#include "KinematicCapsule3D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, KinematicCapsule3D, "KinematicCapsule3D")

	META_DATA_DESC(KinematicCapsule3D)
		BASE_SCENE_ENTITY_PROP(KinematicCapsule3D)
		INT_PROP(KinematicCapsule3D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(KinematicCapsule3D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
	META_DATA_DESC_END()

	void KinematicCapsule3D::Init()
	{
		transform.transformFlag = MoveXYZ;

		Tasks(false)->AddTask(0, this, (Object::Delegate)&KinematicCapsule3D::EditorDraw);
	}

	void KinematicCapsule3D::OnVisiblityChange(bool set)
	{
		if (controller)
		{
			controller->SetActive(set);
		}
	}

	void KinematicCapsule3D::Play()
	{
		PhysControllerDesc desc;
		desc.height = 1.0f;

		auto size = Math::Vector2(100.0f, 150.0f);
		desc.radius = fminf(size.x, size.y) * 0.5f * 0.65f;
		desc.upVector = upVector;
		desc.upVector.Normalize();
		desc.pos = transform.global.Pos();

		controller = GetRoot()->GetPhysScene()->CreateController(desc, 2);
		controller->SetActive(IsVisible());

		bodyData.object = this;
		bodyData.controller = controller;

		controller->SetUserData(&bodyData);
	}

	void KinematicCapsule3D::Move(Math::Vector3 dir, uint32_t ignoreGroup)
	{
		controller->Move(dir * root.GetDeltaTime(), physGroup, ignoreGroup);

		Math::Vector3 pos;
		controller->GetPosition(pos);

		if (affectOnParent && parent)
		{
			auto& parentTrans = parent->GetTransform();
			parentTrans.position = transform.global.Pos();
		}
	}

	void KinematicCapsule3D::EditorDraw(float dt)
	{
		if (IsVisible())
		{

		}
	}
}