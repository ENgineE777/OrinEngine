
#include "Character.h"
#include "Walker.h"
#include "Jumper.h"
#include "Turret.h"
#include "Root/Root.h"

namespace Orin
{
	ENTITYREG(SceneEntity, Character, "MegaBot", "Character")

	META_DATA_DESC(Character)
		BASE_SCENE_ENTITY_PROP(Character)
		ASSET_TEXTURE_PROP(Character, projectile, "Visual", "Projectile")
		ASSET_ANIM_GRAPH_2D_PROP(Character, explostion, "Visual", "Explostion")
		SCENEOBJECT_PROP(Character, camera, "Properties", "camera")
		SCENEOBJECT_PROP(Character, screensRoot, "Properties", "screens root")
	META_DATA_DESC_END()

	Character* Character::player = nullptr;

	bool TestRect(Math::Vector3 rect1Left, Math::Vector3 rect1Right, Math::Vector3 rect2Left, Math::Vector3 rect2Right)
	{
		if (rect1Right.x >= rect2Left.x && rect1Left.x <= rect2Right.x &&
			rect1Right.y >= rect2Left.y && rect1Left.y <= rect2Right.y)
		{
			return true;
		}

		return false;
	}

	void Character::Init()
	{
		ScriptEntity2D::Init();

		Tasks(true)->AddTask(0, this, (Object::Delegate)&Character::Draw);
	}

	void Character::Play()
	{
		player = this;

		ScriptEntity2D::Play();

		animRef = FindChild<AnimGraph2D>();
		controllerRef = FindChild<KinematicCapsule2D>();

		auto& screens = screensRoot->GetChilds();

		for (auto* screen : screens)
		{
			screen->SetVisiblity(false);
		}

		spawnPos = transform.position;

		SetActiveScreen(true);
	}

	void Character::SetSpawnPoint(Math::Vector3 pos)
	{
		spawnRoom = curScreen;
		spawnPos = pos;
		spawnPos.x += screenOffset;
	}

	void Character::SetActiveScreen(bool isRespawn)
	{
		auto& screens = screensRoot->GetChilds();

		if (curScreen != -1)
		{
			screens[curScreen]->SetVisiblity(false);
		}

		if (isRespawn)
		{
			curScreen = spawnRoom;
		}
		else
		{
			curScreen++;
		}

		screens[curScreen]->SetVisiblity(true);

		auto childs = screens[curScreen]->GetChilds();

		for (auto& child : childs)
		{
			if (auto walker = dynamic_cast<Walker*>(child))
			{
				walker->Reset();
			}
			else
			if (auto jumper = dynamic_cast<Jumper*>(child))
			{
				jumper->Reset();
			}
			else
			if (auto turret = dynamic_cast<Turret*>(child))
			{
				turret->Reset();
			}
		}

		auto pos = screens[curScreen]->GetTransform().position;

		screenOffset = pos.x;

		camera->GetTransform().position = pos + Math::Vector3(128.0f, 96.0f, 0.0f);

		if (isRespawn)
		{
			pos = spawnPos;
			ownProjectiles.clear();
			enemyProjectiles.clear();
		}
		else
		{
			pos = transform.position;
			pos.x = screenOffset + 8.0f;
			pos.y += 4.0f;
		}

		controllerRef->SetPosition(pos);
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
			flipped = true;
		}

		if (GetRoot()->GetControls()->DebugKeyPressed("KEY_D", AliasAction::Pressed))
		{
			moveDir.x = 1.0f;
			flipped = false;
		}

		if (ownProjectiles.size() == 0 && GetRoot()->GetControls()->DebugKeyPressed("KEY_F", AliasAction::JustPressed))
		{
			Projectile projectile;
			projectile.dir.x = flipped ? -1.0f : 1.0f;
			projectile.pos = transform.position;
			projectile.pos.y += 14.0f;
			projectile.pos.x += 12.0f * projectile.dir.x;
			projectile.speed = 200.0f;

			ownProjectiles.push_back(projectile);
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

		if (transform.position.x <= screenOffset && moveDir.x < 0.0f)
		{
			moveDir.x = 0.0f;
		}

		controllerRef->Move(moveDir);

		if (transform.position.x > screenOffset + 256.0f - 10.0f)
		{
			SetActiveScreen(false);
		}

		if (transform.position.y < -10.0f)
		{
			SetActiveScreen(true);
		}

		animRef->anim.GotoNode(anim, false);

		for (int i = 0; i < ownProjectiles.size(); i++)
		{
			auto& projectile = ownProjectiles[i];

			projectile.pos += projectile.dir * projectile.speed * dt;

			if (projectile.pos.x < screenOffset || projectile.pos.x > screenOffset + 256.0f)
			{
				ownProjectiles.erase(ownProjectiles.begin() + i);
				i--;
			}
		}

		auto pos = Sprite::ToPixels(transform.GetGlobal().Pos());

		Math::Vector2 size(12, 20.0f);

		leftPos = pos - Math::Vector3(size.x * 0.5f, 0.0f, 0.0f);
		rightPos = pos + Math::Vector3(size.x * 0.5f, size.y, 0.0f);

		for (int i = 0; i < enemyProjectiles.size(); i++)
		{
			auto& projectile = enemyProjectiles[i];

			projectile.pos += projectile.dir * projectile.speed * dt;

			if (projectile.pos.x < screenOffset || projectile.pos.x > screenOffset + 256.0f || projectile.pos.y < 0.0f)
			{
				enemyProjectiles.erase(enemyProjectiles.begin() + i);
				i--;
			}

			Math::Vector2 size = 8.0f;

			auto prjleftPos = projectile.pos - Math::Vector3(size.x * 0.5f, size.y * 0.5f, 0.0f);
			auto prjRightPos = projectile.pos + Math::Vector3(size.x * 0.5f, size.y * 0.5f, 0.0f);

			if (TestRect(leftPos, rightPos, prjleftPos, prjRightPos))
			{
				SetActiveScreen(true);
				break;
			}
		}
	}

	void Character::Draw(float dt)
	{
		for (int i = 0; i < ownProjectiles.size(); i++)
		{
			auto& proj = ownProjectiles[i];

			Transform trasn = transform;
			trasn.position = proj.pos;
			trasn.size = projectile.GetSize();
			projectile.Draw(&trasn, COLOR_WHITE, dt);
		}
		
		for (int i = 0; i < enemyProjectiles.size(); i++)
		{
			auto& proj = enemyProjectiles[i];

			Transform trasn = transform;
			trasn.position = proj.pos;
			trasn.size = projectile.GetSize();
			projectile.Draw(&trasn, COLOR_WHITE, dt);
		}

		if (time2ShowExp >= 0.0f)
		{
			time2ShowExp -= dt;

			if (time2ShowExp < 0.0f)
			{
				time2ShowExp = -1.0f;
			}
			else
			{
				Transform trasn = transform;
				trasn.position = expPos;
				trasn.size = explostion.GetSize();
				explostion.Draw(&trasn, COLOR_WHITE, dt);
			}
		}
	}

	void Character::CheckPlayerDead(Math::Vector3 p1, Math::Vector3 p2)
	{
		if (TestRect(leftPos, rightPos, p1, p2))
		{
			SetActiveScreen(true);
		}
	}

	bool Character::CheckPlayerBullet(Math::Vector3 p1, Math::Vector3 p2, bool oneHitLeft)
	{
		for (int i = 0; i < ownProjectiles.size(); i++)
		{
			auto& proj = ownProjectiles[i];

			Math::Vector2 size = 8.0f;

			auto leftPos = proj.pos - Math::Vector3(size.x * 0.5f, size.y * 0.5f, 0.0f);
			auto rightPos = proj.pos + Math::Vector3(size.x * 0.5f, size.y * 0.5f, 0.0f);

			if (TestRect(leftPos, rightPos, p1, p2))
			{
				ownProjectiles.erase(ownProjectiles.begin() + i);

				if (oneHitLeft)
				{
					time2ShowExp = 0.2f;
					explostion.GotoNode("Exp", true);
					expPos = (p1 + p2) * 0.5f;
				}

				return true;
			}
		}

		return false;
	}

	void Character::AddEnemyProjectile(Math::Vector3 pos, Math::Vector2 dir, float speed)
	{
		Projectile projectile;
		projectile.dir = dir;
		projectile.pos = pos;
		projectile.speed = speed;

		enemyProjectiles.push_back(projectile);
	}
}