#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"
#include "Root/Assets/AssetTexture.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetTileSet : public Asset
	{
		friend class PointerRef<AssetTileSet>;
		friend class AssetTileSetRef;

		static void StartEditTileSet(void* owner);

	public:

		Math::Vector2 camPos;
		float camZoom = 1.0f;
		int selTile = -1;

		struct Tile
		{
			int x = 0;
			int y = 0;
			AssetTextureRef texture;
		};

		int sizeX = 16;
		int sizeY = 16;

		eastl::vector<Tile> tiles;

		META_DATA_DECL_BASE(AssetTileSet)

		void Reload() override;

		void LoadMetaData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		bool IsTileSelected();
		AssetTextureRef GetSelectedTile();
		void SaveMetaData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		#endif
	};

	class CLASS_DECLSPEC AssetTileSetRef : public PointerRef<AssetTileSet>
	{
	public:

		AssetTileSetRef() : PointerRef() {};
		AssetTileSetRef(Asset* setPtr, const char* file, int line) : PointerRef(dynamic_cast<AssetTileSet*>(setPtr), _FL_) {};
		AssetTileSetRef(AssetTileSet* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		AssetTileSetRef& operator=(const AssetTileSetRef& ref)
		{
			Copy(ref);

			return *this;
		}

		void SetupCreatedSceneEntity(SceneEntity* entity);

		Math::Vector2 GetSize();

		void LoadData(JsonReader& loader, const char* name);

	#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
	#endif

	};
}