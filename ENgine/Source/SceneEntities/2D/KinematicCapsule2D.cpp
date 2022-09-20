
#include "KinematicCapsule2D.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, KinematicCapsule2D, "KinematicCapsule2D")

	META_DATA_DESC(KinematicCapsule2D)
		BASE_SCENE_ENTITY_PROP(KinematicCapsule2D)
		INT_PROP(KinematicCapsule2D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(KinematicCapsule2D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
		BOOL_PROP(KinematicCapsule2D, YOriented, true, "Physics", "YOriented", "Affect on parent")
		FLOAT_PROP(KinematicCapsule2D, height, 16.0f, "Physics", "height", "height")
		FLOAT_PROP(KinematicCapsule2D, radius, 16.0f, "Physics", "radius", "radius")
		FLOAT_PROP(KinematicCapsule2D, slopeLimit, 0.0f, "Physics", "slopeLimit", "slopeLimit")
		FLOAT_PROP(KinematicCapsule2D, stepOffset, 0.0f, "Physics", "stepOffset", "stepOffset")
	META_DATA_DESC_END()

	void KinematicCapsule2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
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
		desc.height = Sprite::ToUnits(height);
		desc.radius = Sprite::ToUnits(radius);

		if (!YOriented)
		{
			desc.upVector.Set(0.0f, 0.0f, 1.0f);
		}

		desc.pos = transform.GetGlobal().Pos();
		desc.slopeLimit = slopeLimit;
		desc.stepOffset = Sprite::ToUnits(stepOffset);

		controller = GetRoot()->GetPhysScene()->CreateController(desc, physGroup);
		controller->RestrictZAxis();
		controller->SetActive(IsVisible());

		bodyData.object = this;
		bodyData.controller = controller;

		controller->SetUserData(&bodyData);
	}

	void KinematicCapsule2D::Move(Math::Vector2 dir, uint32_t group)
	{
		controller->Move(Sprite::ToUnits(Math::Vector3(dir)) * root.GetDeltaTime(), group == 0 ? physGroup : group);

		Math::Vector3 pos;
		controller->GetPosition(pos);

		if (affectOnParent && parent)
		{
			parent->GetTransform().position = Sprite::ToPixels(pos);
		}
		else
		{
			Math::Matrix trans = transform.GetGlobal();
			trans.Pos() = pos;

			transform.SetGlobal(trans);
		}
	}

	void KinematicCapsule2D::SetPosition(Math::Vector3 pos)
	{
		if (controller)
		{
			controller->SetPosition(Sprite::ToUnits(pos));
		}
	}

	void KinematicCapsule2D::EditorDraw(float dt)
	{
		if (IsVisible() && !scene->IsPlaying())
		{
			Math::Matrix mat = transform.GetGlobal();
			float unitRadius = Sprite::ToUnits(radius);
			Math::Vector3 dir = YOriented ? mat.Vy() : mat.Vz();

			root.render.DebugSphere(mat.Pos() + dir * unitRadius, COLOR_CYAN, unitRadius);
			root.render.DebugSphere(mat.Pos() + dir * Sprite::ToUnits(radius + height), COLOR_CYAN, unitRadius);
		}
	}
}