
#include "KinematicCapsule2D.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, KinematicCapsule2D, "2D/Physics", "KinematicCapsule2D")

	META_DATA_DESC(KinematicCapsule2D)
		BASE_SCENE_ENTITY_PROP(KinematicCapsule2D)
		INT_PROP(KinematicCapsule2D, physGroup, 1, "Physics", "Physical group", "Physical group")
		BOOL_PROP(KinematicCapsule2D, affectOnParent, false, "Physics", "Affect on parent", "Affect on parent")
		FLOAT_PROP(KinematicCapsule2D, height, 16.0f, "Physics", "height", "height")
		FLOAT_PROP(KinematicCapsule2D, radius, 16.0f, "Physics", "radius", "radius")
		FLOAT_PROP(KinematicCapsule2D, slopeLimit, 0.0f, "Physics", "slopeLimit", "slopeLimit")
		FLOAT_PROP(KinematicCapsule2D, stepOffset, 0.0f, "Physics", "stepOffset", "stepOffset")
	META_DATA_DESC_END()

	void KinematicCapsule2D::Init()
	{
		transform.objectType = ObjectType::Object2D;
		transform.transformFlag = MoveXYZ | RotateZ;

		Tasks(false)->AddTask(0, this, (Object::Delegate)&KinematicCapsule2D::EditorDraw);
	}

	void KinematicCapsule2D::ApplyProperties()
	{
		transform.size.x = 2.f * radius;
		transform.size.y = 2.f * radius + height;
		transform.offset = 0.5f;
	}

	void KinematicCapsule2D::OnVisiblityChange(bool state)
	{
		if (controller)
		{
			controller->SetActive(state);
		}
	}

	Math::Vector3 KinematicCapsule2D::GetTopOffset() const
	{
		const auto offset = transform.size * transform.offset;
		return Sprite::ToUnits(Math::Vector3(-offset.x + radius, offset.y, 0.0f));
	}

	Math::Vector3 KinematicCapsule2D::GetBottomOffset() const
	{
		return GetTopOffset() + Sprite::ToUnits(Math::Vector3(0.0f, -transform.size.y, 0.0f));
	}

	void KinematicCapsule2D::Play()
	{
		Math::Matrix mat = transform.GetGlobal();
		Math::Vector3 upVector = mat.Vy();

		PhysControllerDesc desc;
		desc.height = Sprite::ToUnits(height);
		desc.radius = Sprite::ToUnits(radius);
		desc.upVector = upVector;

		const Math::Vector3 caplusePos = mat * GetBottomOffset();

		desc.pos = caplusePos;
		desc.slopeLimit = slopeLimit;
		desc.stepOffset = Sprite::ToUnits(stepOffset);

		controller = GetRoot()->GetPhysScene()->CreateController(desc, physGroup);
		controller->SetActive(IsVisible());

		bodyData.object = this;
		bodyData.controller = controller;

		controller->SetUserData(&bodyData);

		Tasks(false)->AddTask(-100, this, (Object::Delegate)&KinematicCapsule2D::UpdateParent);
	}

	void KinematicCapsule2D::Move(Math::Vector2 dir, uint32_t group)
	{
		auto prevPos = controller->GetPosition();

		controller->Move(Sprite::ToUnits(Math::Vector3(dir)) * root.GetDeltaTime(), group == 0 ? physGroup : group);

		Math::Vector3 pos = controller->GetPosition();

		if (fabs(prevPos.z - pos.z) > 0.001f)
		{
			pos.z = prevPos.z;
			controller->SetPosition(pos);
		}
	}

	void KinematicCapsule2D::UpdateParent(float dt)
	{
		if (!controller->IsActive())
		{
			return;
		}

		Math::Vector3 pos = controller->GetPosition();

		pos *= Sprite::ToPixels(1.0f);

		if (affectOnParent && parent)
		{
			parent->GetTransform().position = pos - transform.position;
		}
		else
		{
			Math::Matrix trans = transform.GetGlobal();
			trans.Pos() = Sprite::ToUnits(pos);

			transform.SetGlobal(trans);
		}
	}

	Math::Vector3 KinematicCapsule2D::GetPosition()
	{
		if (controller)
		{
			return Sprite::ToPixels(controller->GetPosition());
		}
		return {};
	}

	void KinematicCapsule2D::SetPosition(Math::Vector3 pos)
	{
		if (controller)
		{
			controller->SetPosition(Sprite::ToUnits(pos));

			if (affectOnParent && parent)
			{
				parent->GetTransform().position = pos - transform.position;
			}
		}
	}

	void KinematicCapsule2D::EditorDraw(float dt)
	{
		if (IsVisible() && !scene->IsPlaying())
		{
			DebugDraw();
		}
	}

	void KinematicCapsule2D::DebugDraw()
	{
		Math::Matrix mat = transform.GetGlobal();
		const Math::Vector3 top    = mat * (GetTopOffset() - Sprite::ToUnits(Math::Vector3(0.0f, radius, 0.0f)));
		const Math::Vector3 bottom = mat * (GetBottomOffset() + Sprite::ToUnits(Math::Vector3(0.0f, radius, 0.0f)));

		root.render.DebugSphere(top, COLOR_CYAN_A(0.25f), Sprite::ToUnits(radius));
		root.render.DebugSphere(bottom, COLOR_CYAN_A(0.25f), Sprite::ToUnits(radius));
	}

	void KinematicCapsule2D::Release()
	{
		RELEASE(controller)
		SceneEntity::Release();
	}
}