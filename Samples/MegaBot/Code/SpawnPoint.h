
#pragma once

#include "SceneEntities/Physics/2D/PhysTriger2D.h"

namespace Orin
{
	class SpawnPoint : public PhysTriger2D
	{
	protected:
		eastl::string nextLevel;

	public:

		virtual ~SpawnPoint() = default;

		META_DATA_DECL(SpawnPoint)

		void OnContactStart(int index, SceneEntity* entity, int contactIndex) override;
	};
}
