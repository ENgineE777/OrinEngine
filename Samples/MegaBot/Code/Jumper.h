
#pragma once

#include "SceneEntities/2D/ScriptEntity2D.h"
#include "SceneEntities/2D/AnimGraph2D.h"
#include "SceneEntities/2D/SpriteEntity.h"
#include "SceneEntities/2D/Node2D.h"
#include "SceneEntities/2D/GenericMarker2D.h"

namespace Orin
{
	class Jumper : public ScriptEntity2D
	{
		float jumpHeight = 0.0f;
		float timeflight = 0.0f;
		float timer = 0.0f;
		bool jumpUp = true;
		int health = 5;

		SpriteEntity* animRef = nullptr;

	public:
		META_DATA_DECL_BASE(Jumper)

		void Play() override;
		void Reset();
		void Update(float dt) override;
	};
}
