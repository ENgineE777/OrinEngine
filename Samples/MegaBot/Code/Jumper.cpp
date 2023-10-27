
#include "Jumper.h"
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Jumper, "MegaBot", "Jumper")

	META_DATA_DESC(Jumper)
		BASE_SCENE_ENTITY_PROP(Jumper)
	META_DATA_DESC_END()

	void Jumper::Play()
	{
		ScriptEntity2D::Play();
		
		animRef = FindChild<SpriteEntity>();

		jumpHeight = transform.position.y;

		timeflight = (jumpHeight + 64.0f) / 96.0f;
	}

	void Jumper::Reset()
	{
		auto pos = transform.position;
		pos.y = 0.0f;
		transform.position = pos;
		timer = 0.0f;
		jumpUp = true;

		health = 5;
		SetVisiblity(true);
	}

	void Jumper::Update(float dt)
	{
		if (jumpUp)
		{
			timer += dt;

			if (timer > timeflight)
			{
				timer = timeflight;
				jumpUp = false;
			}
		}
		else
		{
			timer -= dt;

			if (timer < 0.0f)
			{
				timer = 0.0f;
				jumpUp = true;
			}
		}

		float k = timer / timeflight;
		k = 1.0f - (1.0f - k) * (1.0f - k);
		k = 1.0f - k;

		auto pos = transform.position;
		pos.y = jumpHeight - (jumpHeight + 64.0f) * k;
		transform.position = pos;

		auto scale = animRef->GetTransform().scale;
		scale.y = jumpUp ? 1.0f : -1.0f;
		animRef->GetTransform().scale = scale;

		pos = Sprite::ToPixels(transform.GetGlobal().Pos());

		auto size = animRef->texture.GetSize();

		auto leftPos = pos - Math::Vector3(size.x * 0.5f, size.y * 0.5f, 0.0f);
		auto rightPos = pos + Math::Vector3(size.x * 0.5f, size.y * 0.5f, 0.0f);

		Character::player->CheckPlayerDead(leftPos, rightPos);

		if (Character::player->CheckPlayerBullet(leftPos, rightPos, health == 1))
		{
			health--;

			if (health == 0)
			{
				SetVisiblity(false);
			}
		}
	}
}