
#pragma once

#include "Root/Scenes/SceneEntity.h"
#include "Support/MetaData.h"
#include "Support/Sprite.h"
#include "Root/Assets/AssetTileSet.h"
#include "Root/Assets/AssetTexture.h"
#include "Root/Physics/PhysObject.h"
#include "Root/Physics/PhysScene.h"

namespace Oak
{
	class CLASS_DECLSPEC TileMap : public SceneEntity
	{
	public:

		enum class TileHit
		{
			NoHit,
			Tile,
			TileWithCollision
		};

		/**
			\brief Group of a body
		*/

		uint32_t physGroup;

		struct Tile
		{
			int x = 0;
			int y = 0;
			int data = 0;
			AssetTextureRef texture;
		};

		eastl::vector<Tile> tiles;
		eastl::vector<PhysObject*> collition;

		PhysScene::BodyUserData body;

		int drawLevel = 0;
		AssetTileSetRef tileSet;

		META_DATA_DECL(SpriteTile)

	#ifndef DOXYGEN_SKIP

		virtual ~TileMap() = default;

		void Init() override;

		void Play() override;

		void OnVisiblityChange(bool set) override;

		void ApplyProperties() override;
		void Draw(float dt);

		void Load(JsonReader& reader) override;
		void Save(JsonWriter& writer) override;

		TileHit IsPointHitTiles(Math::Vector2 point);

		void Release() override;

	#ifdef OAK_EDITOR
		enum class Mode
		{
			Inactive,
			Place,
			Erase
		};

		Mode mode = Mode::Inactive;

		void SetEditMode(bool ed) override;

		void Copy(SceneEntity* source) override;

		void OnMouseMove(Math::Vector2 ms) override;
		void OnLeftMouseDown() override;
		void OnLeftMouseUp() override;
		void OnRightMouseDown() override;
		void OnRightMouseUp() override;
	#endif

	#endif
	};
}
