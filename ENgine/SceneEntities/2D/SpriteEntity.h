
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class SpriteEntity : public SceneEntity
	{
	public:

		AssetTextureRef texture;

		META_DATA_DECL_BASE(SpriteEntity)

	#ifndef DOXYGEN_SKIP

		SpriteEntity();
		virtual ~SpriteEntity() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}