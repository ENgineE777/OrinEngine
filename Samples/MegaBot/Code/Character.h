
#pragma once

#include "SceneEntities/2D/ScriptEntity2D.h"
#include "SceneEntities/2D/AnimGraph2D.h"
#include "SceneEntities/2D/Node2D.h"
#include "SceneEntities/2D/Camera2D.h"
#include "SceneEntities/Physics/2D/KinematicCapsule2D.h"

namespace Orin
{
	class Character : public ScriptEntity2D
	{
		AnimGraph2D* animRef;
		KinematicCapsule2D* controllerRef;
		float gravity = 0.0f;

		int curScreen = -1;
		float screenOffset = 0.0f;

		struct Projectile
		{
			Math::Vector2 dir;
			Math::Vector3 pos;
			float speed = 0.0f;
		};

		AssetTextureRef projectile;
		AssetAnimGraph2DRef explostion;
		SceneEntityRef<Camera2D> camera;
		SceneEntityRef<Node2D> screensRoot;

		int spawnRoom = 0;
		Math::Vector3 spawnPos;

		bool flipped = false;
		
		eastl::vector<Projectile> ownProjectiles;
		eastl::vector<Projectile> enemyProjectiles;

		float time2ShowExp = -1.0f;
		Math::Vector3 expPos;

		Math::Vector3 leftPos;
		Math::Vector3 rightPos;

	public:

		static Character* player;

		META_DATA_DECL_BASE(SimpleScript)

		void Init() override;

		void SetSpawnPoint(Math::Vector3 pos);
		void SetActiveScreen(bool isRespawn);

		void Play() override;
		void Update(float dt) override;

		void Draw(float dt);

		void CheckPlayerDead(Math::Vector3 p1, Math::Vector3 p2);
		bool CheckPlayerBullet(Math::Vector3 p1, Math::Vector3 p2, bool oneHitLeft);
		void AddEnemyProjectile(Math::Vector3 pos, Math::Vector2 dir, float speed);
	};
}
