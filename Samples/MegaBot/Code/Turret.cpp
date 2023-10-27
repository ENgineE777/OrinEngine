
#include "Turret.h"
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Turret, "MegaBot", "Turret")

	META_DATA_DESC(Turret)
		BASE_SCENE_ENTITY_PROP(Turret)
	META_DATA_DESC_END()

	void Turret::Play()
	{
		ScriptEntity2D::Play();
		
		animRef = FindChild<AnimGraph2D>();
	}

	void Turret::Reset()
	{
		stage = 0;
		timer = 0.0f;
		health = 5;
		animRef->anim.Reset();

		SetVisiblity(true);
	}

	void Turret::Update(float dt)
	{
		auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());

		timer += dt;

		switch (stage)
		{
			case 0:
			{
				if (timer > 0.5f)
				{
					timer = 0.0f;
					stage++;

					animRef->anim.ActivateLink("open");
				}
				break;
			}
			case 1:
			{
				if (timer > 0.5f)
				{
					timer = 0.0f;
					stage++;
				}
				break;
			}
			case 2:
			case 3:
			case 4:
			{
				if (timer > 0.175f)
				{
					Character::player->AddEnemyProjectile(pos + Math::Vector3(-8.0f, 24.0f, 0.0f), { -1.0f, 0.0f }, 200);
					timer = 0.0f;
					stage++;

					if (stage == 5)
					{
						animRef->anim.ActivateLink("close");
					}
				}
				break;
			}
			case 5:
			{
				if (timer > 1.0f)
				{
					timer = 0.0f;
					stage = 0;
				}
				break;
			}
		}

		Math::Vector2 size(16, 22);

		auto leftPos = pos - Math::Vector3(size.x * 0.5f, 0.0f, 0.0f);
		auto rightPos = pos + Math::Vector3(size.x * 0.5f, size.y, 0.0f);

		Character::player->CheckPlayerDead(leftPos, rightPos);

		Character::player->CheckPlayerDead(leftPos, rightPos);

		bool valnurable = stage >= 2 && stage <= 4;

		if (Character::player->CheckPlayerBullet(leftPos, rightPos, valnurable && health == 1) && valnurable)
		{
			health--;

			if (health == 0)
			{
				SetVisiblity(false);
			}
		}
	}
}