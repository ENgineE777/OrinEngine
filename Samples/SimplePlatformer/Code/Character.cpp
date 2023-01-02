
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Character, "Sample", "Character")

	META_DATA_DESC(Character)
		BASE_SCENE_ENTITY_PROP(Character)
	META_DATA_DESC_END()

	void Character::Play()
	{
		ScriptEntity2D::Play();

		animRef = FindChild<AnimGraph2D>();
		controllerRef = FindChild<KinematicCapsule2D>();
	}

	void Character::Update(float dt)
	{
		if (animRef == nullptr || controllerRef == nullptr)
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