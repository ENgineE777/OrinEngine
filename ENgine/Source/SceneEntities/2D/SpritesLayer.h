
#pragma once

#include "PortalMask.h"
#include "Root/Scenes/SceneEntity.h"
#include "Root/Assets/AssetSpritesLayer.h"

namespace Orin
{
	class SpritesLayer : public SceneEntity
	{
		SceneEntityRef<PortalMask> portlaMask;

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

		bool usePortlas = false;

		AssetSpritesLayerRef spritesAsset;

	#ifndef DOXYGEN_SKIP

		virtual ~SpritesLayer() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}
