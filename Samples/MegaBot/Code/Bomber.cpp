
#include "Bomber.h"
#include "Character.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Bomber, "MegaBot", "Bomber")

	META_DATA_DESC(Bomber)
		BASE_SCENE_ENTITY_PROP(Bomber)
	META_DATA_DESC_END()

	void Bomber::Play()
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

		posY = transform.position.y;
	}

	void Bomber::Update(float dt)
	{
		auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());

		if ((moveLeft && pos.x < leftPos.x) ||
			(!moveLeft && pos.x > rightPos.x))
		{
			moveLeft = !moveLeft;
		}

		time2Shoot -= dt;

		if (time2Shoot < 0.0f)
		{
			time2Shoot = 2.0f;
			Character::player->AddEnemyProjectile(pos, { 0.0f, -1.0f }, 200);
		}

		Math::Vector3 moveDir = 0.0f;
		moveDir.x = moveLeft ? -60.0f : 60.0f;

		auto scale = animRef->GetTransform().scale;
		scale.x = moveLeft ? -1.0f : 1.0f;
		animRef->GetTransform().scale = scale;

		wave += dt * 4.0f;

		pos = transform.position + moveDir * dt;
		pos.y = posY + cosf(wave) * 10.0f;

		transform.position = pos;
	}
}