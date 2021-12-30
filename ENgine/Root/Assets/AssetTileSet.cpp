#include "Root/Root.h"
#include "SceneEntities/2D/TileMap.h"


#ifdef OAK_EDITOR
#include "imgui.h"
#endif

namespace Oak
{
#ifdef OAK_EDITOR
	extern void ShowTileSetWindow(AssetTileSet* tileSet);
#endif

	CLASSREG(Asset, AssetTileSet, "AssetTileSet")

	META_DATA_DESC(AssetTileSet)
		INT_PROP(AssetTileSet, sizeX, 16, "Properties", "cell size X", "Horizontal size of a cell")
		INT_PROP(AssetTileSet, sizeY, 16, "Properties", "cell size Y", "Vertical size of a cell")
		CALLBACK_PROP(AssetTileSet, AssetTileSet::StartEditTileSet, "Properties", "Open TileSet Editor")
	META_DATA_DESC_END()

	void AssetTileSet::StartEditTileSet(void* owner)
	{
#ifdef OAK_EDITOR
		ShowTileSetWindow((AssetTileSet*)owner);
#endif
	}

	void AssetTileSet::Reload()
	{
	}

	void AssetTileSet::LoadData(JsonReader& loader)
	{
		if (loader.EnterBlock("tileSet"))
		{
			loader.Read("camPos", camPos);
			loader.Read("camZoom", camZoom);
			loader.Read("selTile", selTile);

			loader.Read("sizeX", sizeX);
			loader.Read("sizeY", sizeY);

			int count = 0;
			loader.Read("count", count);
			tiles.resize(count);

			for (int i = 0; i < count; i++)
			{
				Tile& tile = tiles[i];

				loader.EnterBlock("Tile");

				loader.Read("x", tile.x);
				loader.Read("y", tile.y);
				tile.texture.LoadData(loader, "Texture");

				loader.LeaveBlock();
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	bool AssetTileSet::IsTileSelected()
	{
		return selTile != -1;
	}

	AssetTextureRef AssetTileSet::GetSelectedTile()
	{
		return selTile != -1 ? tiles[selTile].texture : AssetTextureRef();
	}

	void AssetTileSet::SaveData(JsonWriter& saver)
	{
		saver.StartBlock("tileSet");

		saver.Write("camPos", camPos);
		saver.Write("camZoom", camZoom);
		saver.Write("selTile", selTile);

		saver.Write("sizeX", sizeX);
		saver.Write("sizeY", sizeY);

		int count = (int)tiles.size();
		saver.Write("count", count);

		saver.StartArray("Tile");

		for (int i = 0; i < count; i++)
		{
			Tile& tile = tiles[i];

			saver.StartBlock(nullptr);

			saver.Write("x", tile.x);
			saver.Write("y", tile.y);
			tile.texture.SaveData(saver, "Texture");

			saver.FinishBlock();
		}

		saver.FinishArray();

		saver.FinishBlock();
	}

	const char* AssetTileSet::GetSceneEntityType()
	{
		return "TileMap";
	}
	#endif

	void AssetTileSetRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		TileMap* tileMap = dynamic_cast<TileMap*>(entity);

		if (tileMap)
		{
			tileMap->tileSet = *this;

			char str[256];
			StringUtils::Copy(str, 256, Get()->name.c_str());
			StringUtils::RemoveExtension(str);

			entity->SetName(str);
		}
	}

	Math::Vector2 AssetTileSetRef::GetSize()
	{
		return Get() ? Math::Vector2( (float)Get()->sizeX, (float)Get()->sizeX ) : 16.0f;
	}

	void AssetTileSetRef::LoadData(JsonReader& loader, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			eastl::string path;
			if (loader.Read("path", path))
			{
				*this = Oak::root.assets.GetAssetRef<AssetTileSetRef>(path);
			}

			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetTileSetRef::SaveData(JsonWriter& saver, const char* name)
	{
		if (Get())
		{
			saver.StartBlock(name);
			saver.Write("Path", Get()->GetPath().c_str());
			saver.FinishBlock();
		}
	}
	#endif
};