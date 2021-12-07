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
		CALLBACK_PROP(AssetTileSet, AssetTileSet::StartEditTileSet, "Properties", "TileSet Editor")
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
			loader.LeaveBlock();
		}
	}

	#ifdef OAK_EDITOR
	void AssetTileSet::SaveData(JsonWriter& saver)
	{
		saver.StartBlock("tileSet");

		saver.FinishBlock();
	}

	const char* AssetTileSet::GetSceneEntityType()
	{
		return "TileMap";
	}
	#endif

	void AssetTileSetRef::SetupCreatedSceneEntity(SceneEntity* entity)
	{
		TileMap* tileMap = reinterpret_cast<TileMap*>(entity);

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
		return 0.0f;
	}

	void AssetTileSetRef::LoadData(JsonReader& loader, const char* name)
	{
		if (loader.EnterBlock(name))
		{
			eastl::string path;
			if (loader.Read("path", path))
			{
				*this = Oak::root.assets.GetAssetRef<AssetTileSetRef>(path);

				//loader.Read("sliceIndex", sliceIndex);
				//loader.Read("animIndex", animIndex);
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
			//saver.Write("sliceIndex", sliceIndex);
			//saver.Write("animIndex", animIndex);
			saver.FinishBlock();
		}
	}
	#endif
};