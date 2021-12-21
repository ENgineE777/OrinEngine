
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
	META_DATA_DESC_END()

	void KinematicCapsule2D::Init()
	{
		transform.unitsScale = &Sprite::pixelsPerUnit;
		transform.unitsInvScale = &Sprite::pixelsPerUnitInvert;
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
		desc.height = height * Sprite::pixelsPerUnitInvert;
		desc.radius = radius * Sprite::pixelsPerUnitInvert;

		if (!YOriented)
		{
			desc.upVector.Set(0.0f, 0.0f, 1.0f);
		}

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

		pos *= Sprite::pixelsPerUnit;

		if (affectOnParent && parent)
		{
			parent->GetTransform().position = pos;
		}
		else
		{
			Math::Matrix trans = transform.global;
			trans.Pos() = pos;

			transform.global = trans;
		}
	}

	void KinematicCapsule2D::SetPosition(Math::Vector3 pos)
	{
		if (controller)
		{
			controller->SetPosition(pos * Sprite::pixelsPerUnitInvert);
		}
	}

	void KinematicCapsule2D::EditorDraw(float dt)
	{
		if (IsVisible() && !scene->IsPlaying())
		{
			Math::Matrix mat = transform.global;
			Math::Vector3 dir = YOriented ? mat.Vy() : mat.Vz();

			root.render.DebugSphere((mat.Pos() + dir * radius) * Sprite::pixelsPerUnitInvert, COLOR_CYAN, radius * Sprite::pixelsPerUnitInvert);
			root.render.DebugSphere((mat.Pos() + dir * (radius + height)) * Sprite::pixelsPerUnitInvert, COLOR_CYAN, radius * Sprite::pixelsPerUnitInvert);
		}
	}
}