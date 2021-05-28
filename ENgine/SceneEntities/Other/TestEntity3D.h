
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"

namespace Oak
{
	class TestEntity3D : public SceneEntity
	{
	public:

		META_DATA_DECL_BASE(TestEntity3D)

		void Init() override;

		void Draw(float dt);
	};
}