#include "Root/Root.h"
#include "SceneEntities/2D/TileMap.h"


#ifdef ORIN_EDITOR
#include "imgui.h"
#include "Editor/TileSetWindow.h"
#endif

namespace Orin
{
	META_DATA_DESC(AssetTileSet::Tile)
		ENUM_PROP(AssetTileSet::Tile, rotation, 0, "Properties", "rotation", "Rotation of a tile")
			ENUM_ELEM("0", 0)
			ENUM_ELEM("90", 90)
			ENUM_ELEM("180", 180)
			ENUM_ELEM("270", 270)
		ENUM_END
		STRING_PROP(AssetTileSet::Tile, meta, "", "Properties", "Meta")
		ASSET_TEXTURE_PROP(AssetTileSet::Tile, texture, "Properties", "Texture")
	META_DATA_DESC_END()

	CLASSREG(Asset, AssetTileSet, "AssetTileSet")

	META_DATA_DESC(AssetTileSet)
		INT_PROP(AssetTileSet, sizeX, 16, "Properties", "cell size X", "Horizontal size of a cell")
		INT_PROP(AssetTileSet, sizeY, 16, "Properties", "cell size Y", "Vertical size of a cell")
		ASSET_TEXTURE_PROP(AssetTileSet, normal, "Visual", "Normal")
		ASSET_TEXTURE_PROP(AssetTileSet, material, "Visual", "Material")
		AssetTextureRef normal;
		AssetTextureRef material;
	META_DATA_DESC_END()

	void AssetTileSet::Reload()
	{
	}

	void AssetTileSet::LoadMetaData(JsonReader& loader)
	{
		if (loader.EnterBlock("tileSet"))
		{
			loader.Read("camPos", camPos);
			loader.Read("camZoom", camZoom);

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
				loader.Read("rotation", tile.rotation);
				loader.Read("meta", tile.meta);
				tile.texture.LoadData(loader, "Texture");

				loader.LeaveBlock();
			}

			loader.LeaveBlock();
		}
	}

	#ifdef ORIN_EDITOR
	bool AssetTileSet::IsEditable()
	{
		return true;
	}

	void AssetTileSet::ImGuiViewport(bool viewportFocused)
	{
		if (!TileSetWindow::instance || !TileSetWindow::instance ->opened || TileSetWindow::tileSet != this)
		{
			TileSetWindow::StartEdit(this);
		}		
	}

	bool AssetTileSet::IsTilesSelected()
	{
		return selTiles.size() > 0;
	}

	eastl::vector<int> AssetTileSet::GetSelectedTileIndices()
	{
		return selTiles;
	}

	int AssetTileSet::FindIndexByTextureRef(const AssetTextureRef& texture)
	{
		for (int i = 0; i < tiles.size(); i++)
		{
			if (tiles[i].texture == texture)
			{
				return i;
			}
		}

		return -1;
	}

	AssetTextureRef AssetTileSet::GetTileTexture(int index)
	{
		return index >= 0 && index < tiles.size() ? tiles[index].texture : AssetTextureRef();
	}

	float AssetTileSet::GetTileRotation(int index)
	{
		return index >= 0 && index < tiles.size() ? (float)tiles[index].rotation : 0.0f;
	}

	Math::Vector2 AssetTileSet::GetTilePos(int index)
	{
		return index >= 0 && index < tiles.size() ? Math::Vector2((float)tiles[index].x, (float)tiles[index].y) : 0.0f;
	}

	void AssetTileSet::SaveMetaData(JsonWriter& saver)
	{
		saver.StartBlock("tileSet");

		saver.Write("camPos", camPos);
		saver.Write("camZoom", camZoom);

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
			saver.Write("rotation", tile.rotation);
			saver.Write("meta", tile.meta);
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
				*this = root.assets.GetAssetRef<AssetTileSetRef>(path);
			}

			loader.LeaveBlock();
		}
	}

	#ifdef ORIN_EDITOR
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