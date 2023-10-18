
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"
#include "PortalMask.h"

namespace Orin
{
	class SpriteEntity : public SceneEntity
	{
		Math::Vector2 zoneSize;
		Math::Vector2 zoneCenter;

	public:

		Color color;
		Color emmisive;
		float emmisiveIntencity = 1.0f;
		AssetTextureRef texture;
		AssetTextureRef normal;
		AssetTextureRef material;

		/**
			\brief draw level
		*/
		int drawLevel = 0;
		bool noZ = false;
		bool lighten = false;

		int lightGroup = 1;
		bool useRimLight = false;

		bool paralaxInEditor = false;
		Math::Vector2 paralax = 1.0f;

		bool autoTileH = false;
		bool autoTileV = false;
		bool autoCalcTileZone = true;

		bool usePortlas = false;
		SceneEntityRef<PortalMask> portlaMask;

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