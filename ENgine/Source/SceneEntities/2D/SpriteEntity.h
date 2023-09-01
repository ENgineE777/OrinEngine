
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Orin
{
	class SpriteEntity : public SceneEntity
	{
	public:

		Color color;
		AssetTextureRef texture;
		AssetTextureRef normal;
		AssetTextureRef material;

		/**
			\brief draw level
		*/
		int drawLevel = 0;
		bool noZ = false;

		int lightGroup = 1;
		bool useRimLight = false;

		bool paralaxInEditor = false;
		Math::Vector2 paralax = 1.0f;

		META_DATA_DECL_BASE(SpriteEntity)

	#ifndef DOXYGEN_SKIP

		SpriteEntity();
		virtual ~SpriteEntity() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);
		void DrawOccluder(float dt);
	#endif
	};
}