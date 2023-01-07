
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class ParalaxedSpriteEntity : public SceneEntity
	{
	public:

		/**
		\brief Strength of paralax
		*/
		bool paralaxInEditor = false;
		Math::Vector2 paralax = 1.0f;

		AssetTextureRef texture;

		META_DATA_DECL_BASE(ParalaxedSpriteEntity)

	#ifndef DOXYGEN_SKIP

			ParalaxedSpriteEntity();
		virtual ~ParalaxedSpriteEntity() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
	#endif
	};
}