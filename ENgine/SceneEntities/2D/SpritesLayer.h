
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Root/Assets/AssetSpritesLayer.h"

namespace Oak
{
	class SpritesLayer : public SceneEntity
	{
	public:
		META_DATA_DECL(SpritesLayer)

		/**
			\brief draw level
		*/
		int drawLevel = 0;

		/**
		\brief Strength of paralax
		*/
		Math::Vector2 paralax = 1.0f;

		AssetSpritesLayerRef spritesAsset;

	#ifndef DOXYGEN_SKIP

		virtual ~SpritesLayer() = default;

		void Init() override;
		void Draw(float dt);
	#endif
	};
}
