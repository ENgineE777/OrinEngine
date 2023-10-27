
#pragma once

#include "SceneEntities/2D/ScriptEntity2D.h"
#include "SceneEntities/2D/AnimGraph2D.h"
#include "SceneEntities/2D/SpriteEntity.h"
#include "SceneEntities/2D/Node2D.h"
#include "SceneEntities/2D/GenericMarker2D.h"

namespace Orin
{
	class Walker : public ScriptEntity2D
	{
		Math::Vector3 leftPos;
		Math::Vector3 rightPos;

		SpriteEntity* animRef = nullptr;
		bool moveLeft = false;

	public:
		META_DATA_DECL_BASE(Walker)

		void Play() override;
		void Reset();
		void Update(float dt) override;
	};
}
