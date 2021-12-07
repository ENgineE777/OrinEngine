#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetTileSet : public Asset
	{
		friend class PointerRef<AssetTileSet>;
		friend class AssetTileSetRef;

		static void StartEditTileSet(void* owner);

	public:

		int sizeX = 16;
		int sizeY = 16;

		META_DATA_DECL_BASE(AssetTileSet)

		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		#endif
	};

	class CLASS_DECLSPEC AssetTileSetRef : public PointerRef<AssetTileSet>
	{
	public:

		AssetTileSetRef() : PointerRef() {};
		AssetTileSetRef(Asset* setPtr, const char* file, int line) : PointerRef(reinterpret_cast<AssetTileSet*>(setPtr), _FL_) {};
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