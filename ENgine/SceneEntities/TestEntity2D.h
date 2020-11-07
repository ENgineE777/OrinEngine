
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"

namespace Oak
{
	class TestEntity2D : public SceneEntity
	{
	public:

		Sprite::Data data;
		Transform2D transform;
		Sprite::FrameState state;
		Color color;

		META_DATA_DECL_BASE(TestEntity2D)

		void Init() override;
		Transform* GetTransform() override;

		void Draw(float dt);
	};
}