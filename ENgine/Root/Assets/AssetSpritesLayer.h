#pragma once

#include "AssetNavigable2D.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"
#include "Root/Assets/AssetTexture.h"

namespace Oak
{
	class CLASS_DECLSPEC AssetSpritesLayer : public AssetNavigable2D
	{
		friend class PointerRef<AssetSpritesLayer>;
		friend class AssetSpritesLayerRef;

		Math::Vector2 size;
		Math::Vector2 center;

		void CalcInnerValues();

	public:

		META_DATA_DECL_BASE(AssetSpritesLayer)

		struct LayerSprite
		{
			META_DATA_DECL_BASE(LayerSprite)

			Math::Vector2 pos;
			float scale;
			AssetTextureRef texture;
		};

		int selSprite = -1;
		eastl::vector<LayerSprite> sprites;

		void Init() override;

		void Reload() override;

		void Draw(float dt) override;

		void LoadMetaData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		const char* GetSceneEntityType() override;
		virtual void ImGuiMetaProperties();
		virtual void ImGuiProperties();
		#endif
	};

	class CLASS_DECLSPEC AssetSpritesLayerRef : public PointerRef<AssetSpritesLayer>
	{
	public:

		AssetSpritesLayerRef() : PointerRef() {};
		AssetSpritesLayerRef(Asset* setPtr, const char* file, int line) : PointerRef(dynamic_cast<AssetSpritesLayer*>(setPtr), _FL_) {};
		AssetSpritesLayerRef(AssetSpritesLayer* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_) {};

		AssetSpritesLayerRef& operator=(const AssetSpritesLayerRef& ref)
		{
			Copy(ref);

			return *this;
		}

		Math::Vector2 GetSize();

		void SetupCreatedSceneEntity(SceneEntity* entity);

		void LoadData(JsonReader& loader, const char* name);

		void Draw(Math::Vector3 pos, Color clr, float dt);

	#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver, const char* name);
	#endif

	};
}