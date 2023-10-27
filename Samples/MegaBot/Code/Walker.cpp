
#include "Walker.h"
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Walker, "MegaBot", "Walker")

	META_DATA_DESC(Walker)
		BASE_SCENE_ENTITY_PROP(Walker)
	META_DATA_DESC_END()

	void Walker::Reset()
	{
		SetVisiblity(true);
	}

	void Walker::Play()
	{
		ScriptEntity2D::Play();
		
		animRef = FindChild<SpriteEntity>();

		GenericMarker2D* path = FindChild<GenericMarker2D>();

		if (path && path->instances.size() >= 2)
		{
			leftPos = path->instances[0].GetPosition();
			rightPos = path->instances[1].GetPosition();

			if (leftPos.x > rightPos.x)
			{
				leftPos = path->instances[1].GetPosition();
				rightPos = path->instances[0].GetPosition();
			}
		}
	}

	void Walker::Update(float dt)
	{		
		Math::Vector3 moveDir = 0.0f;
		moveDir.x = moveLeft ? -70.0f : 70.0f;

		auto scale = animRef->GetTransform().scale;
		scale.x = moveLeft ? -1.0f : 1.0f;
		animRef->GetTransform().scale = scale;

		transform.position = transform.position + moveDir * dt;

		auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());

		if ((moveLeft && pos.x < leftPos.x) ||
			(!moveLeft && pos.x > rightPos.x))
		{
			moveLeft = !moveLeft;
		}

		auto size = animRef->texture.GetSize();

		auto leftPos = pos - Math::Vector3(size.x * 0.5f, 0.0f, 0.0f);
		auto rightPos = pos + Math::Vector3(size.x * 0.5f, size.y, 0.0f);

		Character::player->CheckPlayerDead(leftPos, rightPos);

		if (Character::player->CheckPlayerBullet(leftPos, rightPos, true))
		{
			SetVisiblity(false);
		}
	}
}