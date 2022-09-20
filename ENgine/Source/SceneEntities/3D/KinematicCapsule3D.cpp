
#include "KinematicCapsule3D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, KinematicCapsule3D, "KinematicCapsule3D")

	META_DATA_DESC(KinematicCapsule3D)
		BASE_SCENE_ENTITY_PROP(KinematicCapsule3D)
		INT_PROP(KinematicCapsule3D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(KinematicCapsule3D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
		FLOAT_PROP(KinematicCapsule3D, height, 0.5f, "Physics", "height", "height")
		FLOAT_PROP(KinematicCapsule3D, radius, 0.5f, "Physics", "radius", "radius")
		FLOAT_PROP(KinematicCapsule3D, slopeLimit, cosf(Math::Radian * 20.0f), "Physics", "slopeLimit", "slopeLimit")
		FLOAT_PROP(KinematicCapsule3D, stepOffset, 0.5f, "Physics", "stepOffset", "stepOffset")
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
		desc.pos = transform.GetGlobal().Pos();
		desc.slopeLimit = slopeLimit;
		desc.stepOffset = stepOffset;

		controller = GetRoot()->GetPhysScene()->CreateController(desc, physGroup);
		controller->SetActive(IsVisible());

		bodyData.object = this;
		bodyData.controller = controller;

		controller->SetUserData(&bodyData);
	}

	void KinematicCapsule3D::Move(Math::Vector3 dir, uint32_t group)
	{
		controller->Move(dir * root.GetDeltaTime(), group == 0 ? physGroup : group);

		Math::Vector3 pos = controller->GetFootPosition();

		if (affectOnParent && parent)
		{
			auto& parentTrans = parent->GetTransform();
			parentTrans.position = transform.GetGlobal().Pos();
		}
	}

	void KinematicCapsule3D::EditorDraw(float dt)
	{
		if (IsVisible() && !scene->IsPlaying())
		{
			Math::Matrix mat = transform.GetGlobal();
			Math::Vector3 dir = mat.Vy();

			root.render.DebugSphere((mat.Pos() + dir * radius), COLOR_CYAN, radius);
			root.render.DebugSphere((mat.Pos() + dir * (radius + height)), COLOR_CYAN, radius);
		}
	}
}