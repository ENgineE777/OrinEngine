#pragma once

#include "Asset.h"
#include "root/Render/Render.h"
#include "support/Sprite.h"

namespace Oak
{
	class AssetTexture : public Asset
	{
		friend class PointerRef<AssetTexture>;

		TextureFilter textureFilter = TextureFilter::Linear;
		TextureAddress texturMode = TextureAddress::Wrap;

		static void StartEditAssetTexture(void* owner);

	public:

		struct Slice
		{
			eastl::string name;
			bool isNineSliced = 0;
			Math::Vector2 pos = 0.0f;
			Math::Vector2 size = 0.0f;
			Math::Vector2 upLeftOffset = 0.0f;
			Math::Vector2 downRightOffset = 0.0f;
			Math::Vector2 offset = 0.5f;
		};

		TextureRef texture;
		Math::Vector2 size;
		eastl::vector<Slice> slices;

		META_DATA_DECL_BASE(AssetTexture)

		TextureRef GetTexture();
		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		void SetupCreatedSceneEntity(SceneEntity* entity) override;
		#endif
	};

	class AssetTextureRef : public PointerRef<AssetTexture>
	{
	public:

		AssetTextureRef() : PointerRef()
		{

		};

		AssetTextureRef(AssetTexture* setPtr, const char* file, int line) : PointerRef(setPtr, _FL_)
		{

		};

		int sliceIndex = -1;

		void Draw(Transform* trans, Color clr);
	};
}