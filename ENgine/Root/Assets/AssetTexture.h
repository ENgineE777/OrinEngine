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

		META_DATA_DECL_BASE(AssetTexture)

		Sprite::Sheet spriteSheet;

		TextureRef GetTexture();
		void Reload() override;

		void LoadData(JsonReader& loader) override;

		#ifdef OAK_EDITOR
		void SaveData(JsonWriter& saver) override;
		const char* GetSceneEntityType() override;
		void SetupCreatedSceneEntity(SceneEntity* entity) override;
		#endif
	};

	typedef PointerRef<AssetTexture> AssetTextureRef;
}