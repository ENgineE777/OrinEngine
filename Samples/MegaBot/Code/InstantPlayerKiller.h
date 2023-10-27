
#pragma once

#include "SceneEntities/Physics/2D/PhysTriger2D.h"

namespace Orin
{
	class InstantPlayerKiller : public PhysTriger2D
	{
	public:

		virtual ~InstantPlayerKiller() = default;

		META_DATA_DECL(InstantPlayerKiller)

		void Init() override;
		void OnContactStart(int index, SceneEntity* entity, int contactIndex) override;
	};
}
