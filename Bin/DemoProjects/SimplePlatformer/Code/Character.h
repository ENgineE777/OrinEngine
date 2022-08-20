
#pragma once

#include "SceneEntities/2D/ScriptEntity2D.h"
#include "SceneEntities/2D/AnimGraph2D.h"
#include "SceneEntities/2D/KinematicCapsule2D.h"

namespace Oak
{
	class Character : public ScriptEntity2D
	{
		SceneEntityRef<AnimGraph2D> animRef;
		SceneEntityRef<KinematicCapsule2D> controllerRef;
		float gravity = 0.0f;

	public:
		META_DATA_DECL_BASE(SimpleScript)

		void Update(float dt) override;
	};
}
