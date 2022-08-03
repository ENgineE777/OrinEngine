#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"
#include "Root/Assets/AssetTexture.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetSpriteLayer : public Asset
	{
		friend class PointerRef<AssetSpriteLayer>;
		friend class AssetTileSetRef;

	public:

		META_DATA_DECL_BASE(AssetSpriteLayer)

		struct LayerSprite
		{
			META_DATA_DECL_BASE(LayerSprite)

			Math::Vector2 pos;
			Math::Vector2 size;
			AssetTextureRef texture;
		};

		int selSprite = -1;
		eastl::vector<LayerSprite> sprites;

		void Init() override;

		void Reload() override;

		void Draw(float dt);

		#ifdef OAK_EDITOR
		const char* GetSceneEntityType() override;
		#endif
	};

	class CLASS_DECLSPEC AssetSpriteLayerRef : public PointerRef<AssetSpriteLayer>
	{
	public:

		AssetSpriteLayerRef() : PointerRef() {};
		AssetSpriteLayerRef(Asset* setPtr, const char* file, int line) : PointerRef(dynamic_cast<AssetSpriteLayer*>(setPtr), _FL_) {};
		AssetSpriteLayerRef(AssetSpriteLayer* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		AssetSpriteLayerRef& operator=(const AssetSpriteLayerRef& ref)
		{
			Copy(ref);

			return *this;
		}

		void SetupCreatedSceneEntity(SceneEntity* entity);

		void LoadData(JsonReader& loader, const char* name);

	#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
	#endif

	};
}