
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "root/Assets/AssetTexture.h"

namespace Oak
{
	class TileMap : public SceneEntity
	{
	public:

		struct Tile
		{
			int x = 0;
			int y = 0;
		};

		eastl::vector<Tile> instances;

		int drawLevel = 0;
		AssetTextureRef texture;

		META_DATA_DECL(SpriteTile)

	#ifndef DOXYGEN_SKIP

		virtual ~TileMap() = default;

		void Init() override;
		void ApplyProperties() override;
		void Draw(float dt);

	#ifdef OAK_EDITOR
		enum class Mode
		{
			Inactive,
			Place,
			Erase
		};

		Mode mode = Mode::Inactive;

		void SetEditMode(bool ed) override;

		void OnMouseMove(Math::Vector2 ms) override;
		void OnLeftMouseDown() override;
		void OnLeftMouseUp() override;
		void OnRightMouseDown() override;
		void OnRightMouseUp() override;
	#endif

	#endif
	};
}
