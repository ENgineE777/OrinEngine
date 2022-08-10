
#include "Character.h"
#include "Root/Root.h"

namespace Oak
{
	CLASSREG(SceneEntity, Character, "Character")

	META_DATA_DESC(Character)
		BASE_SCENE_ENTITY_PROP(Character)
		SCENEOBJECT_PROP(Character, animRef, "Properties", "anim")
		SCENEOBJECT_PROP(Character, controllerRef, "Properties", "controller")
	META_DATA_DESC_END()

	void Character::Update(float dt)
	{
		if (!animRef.entity || !controllerRef.entity)
		{
			return;
		}

		Math::Vector2 moveDir = 0.0f;
		float runSpeed = 120.0f;
		const char* anim = "idle";

		if (GetRoot()->GetControls()->DebugKeyPressed("KEY_A", AliasAction::Pressed))
		{
			moveDir.x = -1.0f;
		}

		if (GetRoot()->GetControls()->DebugKeyPressed("KEY_D", AliasAction::Pressed))
		{
			moveDir.x = 1.0f;
		}

		if (fabs(moveDir.x) > 0.1f)
		{
			animRef->GetTransform().scale = Math::Vector3(moveDir.x, 1.0f, 1.0f);
		}

		moveDir.x *= runSpeed;

		gravity -= dt * 400.0f;

		if (gravity < -150.0f)
		{
			gravity = -150.0f;
		}

		moveDir.y = gravity;

		if (gravity > 0.0f)
		{
			anim = "jump";
		}
		else
		{
			anim = "fall";
		}

		if (gravity < 0.0f && controllerRef->controller->IsColliding(PhysController::CollideDown))
		{
			gravity = 0.0f;

			if (fabs(moveDir.x) > 0.1f)
			{
				anim = "run";
			}
			else
			{
				anim = "idle";
			}

			if (GetRoot()->GetControls()->DebugKeyPressed("KEY_W", AliasAction::JustPressed))
			{
				gravity = 200.0f;
			}
		}

		controllerRef->Move(moveDir);

		animRef->anim.GotoNode(anim, false);
	}
}