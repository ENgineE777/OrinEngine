
#pragma once

#include "SceneEntities/2D/ScriptEntity2D.h"
#include "SceneEntities/2D/AnimGraph2D.h"
#include "SceneEntities/2D/SpriteEntity.h"
#include "SceneEntities/2D/Node2D.h"
#include "SceneEntities/2D/GenericMarker2D.h"

namespace Orin
{
	class Turret : public ScriptEntity2D
	{
		int stage = 0;

		float timer = 0.0f;
		int health = 10;

		AnimGraph2D* animRef = nullptr;

	public:
		META_DATA_DECL_BASE(Turret)

		void Play() override;
		void Reset();
		void Update(float dt) override;
	};
}
