#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"
#include "Root/Assets/AssetTexture.h"

namespace Orin
{
	class CLASS_DECLSPEC AssetTileSet : public Asset
	{
		friend class PointerRef<AssetTileSet>;
		friend class AssetTileSetRef;

	public:

		Math::Vector2 camPos;
		float camZoom = 1.0f;
		eastl::vector<int> selTiles;

		struct Tile
		{
			META_DATA_DECL_BASE(Tile)

			int x = 0;
			int y = 0;
			int rotation = 0;
			bool flipH = false;
			bool flipV = false;
			Color emmisive = COLOR_WHITE;
			float emmisiveIntencity = 1.0f;
			eastl::string meta;
			AssetTextureRef texture;
		};

		AssetTextureRef normal;
		AssetTextureRef material;

		int sizeX = 16;
		int sizeY = 16;

		eastl::vector<Tile> tiles;

		META_DATA_DECL_BASE(AssetTileSet)

		void Reload() override;

		void LoadMetaData(JsonReader& loader) override;

		#ifdef ORIN_EDITOR
		bool IsEditable() override;
		bool IsTilesSelected();
		void ImGuiViewport(bool viewportFocused) override;
		eastl::vector<int> GetSelectedTileIndices();
		int FindIndexByTextureRef(const AssetTextureRef& texture);
		AssetTextureRef GetTileTexture(int index);
		Math::Vector2 GetTilePos(int index);
		float GetTileRotation(int index);
		bool IsFlippedHorrizontaly(int index);
		bool IsFlippedVertically(int index);
		Color GetEmmisive(int index);
		float GetEmmisiveIntencity(int index);

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

	#ifdef ORIN_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
	#endif

	};
}