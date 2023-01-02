
#pragma once

#include "SceneEntities/2D/ScriptEntity2D.h"
#include "SceneEntities/2D/AnimGraph2D.h"
#include "SceneEntities/2D/KinematicCapsule2D.h"

namespace Orin
{
	class Character : public ScriptEntity2D
	{
		AnimGraph2D* animRef;
		KinematicCapsule2D* controllerRef;
		float gravity = 0.0f;

	public:
		META_DATA_DECL_BASE(SimpleScript)

		void Play() override;
		void Update(float dt) override;
	};
}
